#define ACEERR_MEM      1
#define ACEERR_FILES    2
#define ACEERR_FOUND    3
#define ACEERR_FULL     4
#define ACEERR_OPEN     5
#define ACEERR_READ     6
#define ACEERR_WRITE    7
#define ACEERR_CLINE    8
#define ACEERR_CRC      9
#define ACEERR_OTHER   10
#define ACEERR_EXISTS  11
#define ACEERR_END      128
#define ACEERR_HANDLE   129
#define ACEERR_CONSTANT 130
#define ACEERR_NOPASSW  131
#define ACEERR_METHOD   132
#define ACEERR_USER   255

#define ACEOPEN_LIST    0
#define ACEOPEN_EXTRACT 1

#define ACECMD_SKIP     0
#define ACECMD_TEST     1
#define ACECMD_EXTRACT  2

#define ACEVOL_REQUEST  0
#define ACEVOL_OPENED   1


#define ACESEL_YES      0
#define ACESEL_ALWAYS   1
#define ACESEL_NO       2
#define ACESEL_CANCEL   3

#define ACECMT_OK       0
#define ACECMT_SMALLBUF 1
#define ACECMT_NONE   255

#pragma pack(1)          // structures are byte-aligned!!

struct ACEHeaderData
{
  char ArcName[260];
  char FileName[260];
  UINT Flags;
  UINT PackSize;
  UINT UnpSize;
  UINT FileCRC;
  UINT FileTime;
  UINT Method;
  UINT QUAL;
  UINT FileAttr;
  char *CmtBuf;
  UINT CmtBufSize;
  UINT CmtSize;
  UINT CmtState;
};

struct ACEOpenArchiveData
{
  char *ArcName;
  UINT OpenMode;
  UINT OpenResult;
  UINT Flags;
  UINT Host;
  char AV[51];
  char *CmtBuf;
  UINT CmtBufSize;
  UINT CmtSize;
  UINT CmtState;
  int (*ChangeVolProc)(char *ArcName,int Mode);
  int (*ProcessDataProc)(unsigned char *Addr,int Size);
};

#pragma pack()

HANDLE __stdcall ACEOpenArchive(struct ACEOpenArchiveData *ArchiveData);
int __stdcall ACECloseArchive(HANDLE hArcData);
int __stdcall ACEReadHeader(HANDLE hArcData,struct ACEHeaderData *HeaderData);
int __stdcall ACEProcessFile(HANDLE hArcData,int Operation,char *DestPath);
int __stdcall ACESetPassword(HANDLE hArcData,char *Password);
