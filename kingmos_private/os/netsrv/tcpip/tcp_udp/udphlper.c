




// -------------------------------------------------------------
// UDP��Ϣ �Ĳ���
// -------------------------------------------------------------

DWORD
WINAPI
GetUdpStatistics(
    OUT PMIB_UDPSTATS   pStats
    );


DWORD
WINAPI
GetUdpTable(
    OUT    PMIB_UDPTABLE pUdpTable,
    IN OUT PDWORD        pdwSize,
    IN     BOOL          bOrder
    );



