





// -------------------------------------------------------------
// TCP信息 的操作
// -------------------------------------------------------------

DWORD
WINAPI
GetTcpStatistics(
    OUT PMIB_TCPSTATS   pStats
    );

DWORD
WINAPI
GetTcpTable(
    OUT    PMIB_TCPTABLE pTcpTable,
    IN OUT PDWORD        pdwSize,
    IN     BOOL          bOrder
    );

DWORD
WINAPI
SetTcpEntry(
    IN PMIB_TCPROW pTcpRow
    );


