#include <iostream>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>
#include <string>
#include <sstream>
#include "TcpMessage.cpp"

extern std::vector<TcpMessage*>& getTcpMessageList();

void printMessageData(const TcpMessageData& tmd) {
    std::cout << "Ethernet header source MAC: " << tmd.src_mac << std::endl;
    std::cout << "Source IP: " << tmd.src_ip << std::endl;
    std::cout << "Destination IP: " << tmd.dst_ip<< std::endl;
    std::cout << "Source port: " << tmd.src_port << std::endl;
    std::cout << "Destination port: " << tmd.dst_port << std::endl;
    std::cout << "Payload " << tmd.data.size() << ": ";
    for (int i = 0; i < tmd.data.size(); i++) {
        if (i % 16 == 0)
            printf("\n");
        printf("%c ", (unsigned char)tmd.data[i]);
    }
    std::cout << std::endl;
}


// 回调函数，当捕获到数据包时调用
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    TcpMessageData tmd;

    // 计算TCP payload 长度以及获取payload指针
    const struct ip* ipHeader = (struct ip*)(packet + 14);
    int ipHeaderLength = ipHeader->ip_hl * 4;
    const struct tcphdr* tcpHeader = (struct tcphdr*)(packet + 14 + ipHeaderLength);
    int tcpHeaderLength = tcpHeader->th_off * 4;
    char* payload = (char *)(packet + 14 + ipHeaderLength + tcpHeaderLength);
    int payloadLength = pkthdr->len - (14 + ipHeaderLength + tcpHeaderLength);
    if (payloadLength > 0) {
        for (int i = 0; i < payloadLength; i++) {
            tmd.data.push_back((uint8_t)payload[i]);
        }
    }
    
    // 解析以太网头部
    struct ether_header *eth_header;
    std::stringstream mac_stream;
    eth_header = (struct ether_header *) packet;
    for (int i = 0; i < ETHER_ADDR_LEN; i++) {
        mac_stream << std::hex << (int)eth_header->ether_shost[i];
        if (i < ETHER_ADDR_LEN - 1) {
            mac_stream << ":";
        }
    }
    tmd.src_mac = mac_stream.str();
    

    // 解析IP头部
    struct ip *ip_header;
    ip_header = (struct ip *)(packet + sizeof(struct ether_header));
    tmd.src_ip = inet_ntoa(ip_header->ip_src);
    tmd.dst_ip =  inet_ntoa(ip_header->ip_dst);

    // 解析TCP头部
    if (ip_header->ip_p == IPPROTO_TCP) {
        struct tcphdr *tcp_header;
        tcp_header = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip));
        tmd.src_port = ntohs(tcp_header->source);
        tmd.dst_port = ntohs(tcp_header->dest);
    }

    std::vector<TcpMessage*>& tm = getTcpMessageList();
    // Check TCP flags
    if (tcpHeader->syn && !tcpHeader->ack) {
        for (size_t i = 0; i < tm.size(); i++) {
            tm[i]->established(tmd);
        }
    } else if (tcpHeader->fin) {
        for (size_t i = 0; i < tm.size(); i++) {
            tm[i]->close(tmd);
        };
    }
    // 如果payload大于0则分析payload
    if(payloadLength > 0) {
        for (size_t i = 0; i < tm.size(); i++) {
            tm[i]->analysis(tmd);
        }
    }
}


class TcpCaptureTest : public TcpMessage
{
private:
    
public:
    TcpCaptureTest() = default;
    ~TcpCaptureTest() = default;
    virtual void analysis(const TcpMessageData& tmd) {
        printMessageData(tmd);
        std::cout << "analysis" << std::endl;
        std::cout << std::endl;
    }
    virtual void established(const TcpMessageData& tmd) {
        printMessageData(tmd);
        std::cout << "established" << std::endl;
        std::cout << std::endl;
    };
    virtual void close(const TcpMessageData& tmd) {
        printMessageData(tmd);
        std::cout << "close" << std::endl;
        std::cout << std::endl;
    };
};




int main(int argc, char **argv) {
    char *dev = nullptr;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct bpf_program fp;
    std::string filter_exp = "tcp dst port 12345";
    bpf_u_int32 net;

    registerTcpMessage(static_cast<TcpMessage*>(new TcpCaptureTest));

    dev = argv[1];

    // 打开设备
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        std::cerr << "Couldn't open device " << dev << ": " << errbuf << std::endl;
        return 1;
    }

    // 编译过滤器表达式
    if (pcap_compile(handle, &fp, filter_exp.c_str(), 0, net) == -1) {
        std::cerr << "Couldn't parse filter " << filter_exp << ": " << pcap_geterr(handle) << std::endl;
        return 1;
    }

    // 应用过滤器
    if (pcap_setfilter(handle, &fp) == -1) {
        std::cerr << "Couldn't install filter " << filter_exp << ": " << pcap_geterr(handle) << std::endl;
        return 1;
    }

    // 开始捕获数据包
    pcap_loop(handle, -1, packet_handler, NULL);

    // 关闭会话
    pcap_close(handle);

    return 0;
}
