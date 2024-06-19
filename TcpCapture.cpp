#include <iostream>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include "TcpCapture.hpp"

extern std::vector<TcpCapture*>& getTcpMessageList();

void printMessage(const TcpMessage& tmd) {
    std::cout << "Timestamp: " << tmd.time_sec << "." << tmd.time_us << std::endl;
    std::cout << "Ethernet header source MAC: " << tmd.src_mac << std::endl;
    std::cout << "Source IP: " << tmd.src_ip << std::endl;
    std::cout << "Destination IP: " << tmd.dst_ip<< std::endl;
    std::cout << "Source port: " << tmd.src_port << std::endl;
    std::cout << "Destination port: " << tmd.dst_port << std::endl;
    std::cout << "Payload " << tmd.data.size() << ": ";
    for (int i = 0; i < tmd.data.size(); i++) {
        if (i % 16 == 0)
            printf("\n");
        printf("%02x ", (unsigned char)tmd.data[i]);
    }
    std::cout << std::endl;
    std::cout << std::endl;
}


// 回调函数，当捕获到数据包时调用
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    TcpMessage tm;

    // 获取时间戳
    tm.time_sec = pkthdr->ts.tv_sec;
    tm.time_us = pkthdr->ts.tv_usec;

    // 计算TCP payload 长度以及获取payload指针
    const struct ip* ip_header = (struct ip *)(packet + sizeof(struct ether_header));;
    int ip_header_length = ip_header->ip_hl * 4;
    const struct tcphdr* tcpHeader = (struct tcphdr*)(packet + 14 + ip_header_length);
    int tcp_header_length = tcpHeader->th_off * 4;
    char* payload = (char *)(packet + 14 + ip_header_length + tcp_header_length);
    int payload_length = pkthdr->len - (14 + ip_header_length + tcp_header_length);

    // 计算 padding 长度
    int ip_total_len = ntohs(ip_header->ip_len);
    int ethernet_payload_len = pkthdr->len - sizeof(struct ether_header);
    int padding_len = ethernet_payload_len - ip_total_len;

    // 将padding部分去除
    if (padding_len > 0){
        payload_length -= padding_len;
    }
    
    if (payload_length > 0) {
        for (int i = 0; i < payload_length; i++) {
            tm.data.push_back((uint8_t)payload[i]);
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
    tm.src_mac = mac_stream.str();
    
    // 解析IP头部
    tm.src_ip = inet_ntoa(ip_header->ip_src);
    tm.dst_ip =  inet_ntoa(ip_header->ip_dst);

    // 解析TCP头部
    if (ip_header->ip_p == IPPROTO_TCP) {
        struct tcphdr *tcp_header;
        tcp_header = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip));
        tm.src_port = ntohs(tcp_header->source);
        tm.dst_port = ntohs(tcp_header->dest);
    }

    std::vector<TcpCapture*>& tc = getTcpMessageList();
    // Check TCP flags
    if (tcpHeader->syn && !tcpHeader->ack) {
        for (size_t i = 0; i < tc.size(); i++) {
            tc[i]->established(tm);
        }
    } else if (tcpHeader->fin) {
        for (size_t i = 0; i < tc.size(); i++) {
            tc[i]->close(tm);
        };
    } else if(payload_length > 0) {
        for (size_t i = 0; i < tc.size(); i++) {
            tc[i]->analysis(tm);
        }
    }
}


int startTcpCapture(const std::string& dev, const std::string& filter) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct bpf_program fp;
    std::string filter_exp = filter;
    bpf_u_int32 net;

    // 打开设备
    handle = pcap_open_live(dev.c_str(), BUFSIZ, 1, 1000, errbuf);
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
