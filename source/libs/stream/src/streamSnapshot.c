/*
 * Copyright (c) 2019 TAOS Data, Inc. <jhtao@taosdata.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the GNU Affero General Public License, version 3
 * or later ("AGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "streamSnapshot.h"
#include "query.h"
#include "rocksdb/c.h"
#include "streamBackendRocksdb.h"
#include "streamInt.h"
#include "tcommon.h"

enum SBackendFileType {
  ROCKSDB_OPTIONS_TYPE = 1,
  ROCKSDB_MAINFEST_TYPE = 2,
  ROCKSDB_SST_TYPE = 3,
  ROCKSDB_CURRENT_TYPE = 4,
  ROCKSDB_CHECKPOINT_META_TYPE = 5,
};

typedef struct SBackendFileItem {
  char*   name;
  int8_t  type;
  int64_t size;
} SBackendFileItem;
typedef struct SBackendFile {
  char*   pCurrent;
  char*   pMainfest;
  char*   pOptions;
  SArray* pSst;
  char*   pCheckpointMeta;
  char*   path;

} SBanckendFile;

typedef struct SBackendSnapFiles2 {
  char*   pCurrent;
  char*   pMainfest;
  char*   pOptions;
  SArray* pSst;
  char*   pCheckpointMeta;
  char*   path;

  int64_t   checkpointId;
  int64_t   seraial;
  int64_t   offset;
  TdFilePtr fd;
  int8_t    filetype;
  SArray*   pFileList;
  int32_t   currFileIdx;

} SBackendSnapFile2;
struct SStreamSnapHandle {
  void*          handle;
  SBanckendFile* pBackendFile;
  int64_t        checkpointId;
  int64_t        seraial;
  int64_t        offset;
  TdFilePtr      fd;
  int8_t         filetype;
  SArray*        pFileList;
  int32_t        currFileIdx;

  SArray* pBackendSnapSet;
  int32_t currIdx;
};
struct SStreamSnapBlockHdr {
  int8_t  type;
  int8_t  flag;
  int64_t index;
  char    name[128];
  int64_t totalSize;
  int64_t size;
  uint8_t data[];
};
struct SStreamSnapReader {
  void*             pMeta;
  int64_t           sver;
  int64_t           ever;
  SStreamSnapHandle handle;
  int64_t           checkpointId;
};
struct SStreamSnapWriter {
  void*             pMeta;
  int64_t           sver;
  int64_t           ever;
  SStreamSnapHandle handle;
};
const char*    ROCKSDB_OPTIONS = "OPTIONS";
const char*    ROCKSDB_MAINFEST = "MANIFEST";
const char*    ROCKSDB_SST = "sst";
const char*    ROCKSDB_CURRENT = "CURRENT";
const char*    ROCKSDB_CHECKPOINT_META = "CHECKPOINT";
static int64_t kBlockSize = 64 * 1024;

int32_t streamSnapHandleInit(SStreamSnapHandle* handle, char* path, int64_t chkpId, void* pMeta);
void    streamSnapHandleDestroy(SStreamSnapHandle* handle);

// static void streamBuildFname(char* path, char* file, char* fullname)

#define STREAM_ROCKSDB_BUILD_FULLNAME(path, file, fullname) \
  do {                                                      \
    sprintf(fullname, "%s%s%s", path, TD_DIRSEP, file);     \
  } while (0)

int32_t streamGetFileSize(char* path, char* name, int64_t* sz) {
  int ret = 0;

  char* fullname = taosMemoryCalloc(1, strlen(path) + 32);
  sprintf(fullname, "%s%s%s", path, TD_DIRSEP, name);

  ret = taosStatFile(fullname, sz, NULL, NULL);
  taosMemoryFree(fullname);

  return ret;
}

TdFilePtr streamOpenFile(char* path, char* name, int32_t opt) {
  char fullname[256] = {0};
  STREAM_ROCKSDB_BUILD_FULLNAME(path, name, fullname);
  return taosOpenFile(fullname, opt);
}

int32_t streamBackendGetSnapInfo(void* arg, char* path, int64_t chkpId) { return taskBackendBuildSnap(arg, chkpId); }

void snapFileDebugInfo(SBackendSnapFile2* pSnapFile) {
  char* buf = taosMemoryCalloc(1, 512);
  sprintf(buf, "[current: %s,", pSnapFile->pCurrent);
  sprintf(buf + strlen(buf), "MANIFEST: %s,", pSnapFile->pMainfest);
  sprintf(buf + strlen(buf), "options: %s,", pSnapFile->pOptions);

  for (int i = 0; i < taosArrayGetSize(pSnapFile->pSst); i++) {
    char* name = taosArrayGetP(pSnapFile->pSst, i);
    sprintf(buf + strlen(buf), "%s,", name);
  }
  sprintf(buf + strlen(buf) - 1, "]");

  qInfo("%s get file list: %s", STREAM_STATE_TRANSFER, buf);
  taosMemoryFree(buf);
}

int32_t snapFileCvtMeta(SBackendSnapFile2* pSnapFile) {
  SBackendFileItem item;
  // current
  item.name = pSnapFile->pCurrent;
  item.type = ROCKSDB_CURRENT_TYPE;
  streamGetFileSize(pSnapFile->path, item.name, &item.size);
  taosArrayPush(pSnapFile->pFileList, &item);

  // mainfest
  item.name = pSnapFile->pMainfest;
  item.type = ROCKSDB_MAINFEST_TYPE;
  streamGetFileSize(pSnapFile->path, item.name, &item.size);
  taosArrayPush(pSnapFile->pFileList, &item);

  // options
  item.name = pSnapFile->pOptions;
  item.type = ROCKSDB_OPTIONS_TYPE;
  streamGetFileSize(pSnapFile->path, item.name, &item.size);
  taosArrayPush(pSnapFile->pFileList, &item);
  // sst
  for (int i = 0; i < taosArrayGetSize(pSnapFile->pSst); i++) {
    char* sst = taosArrayGetP(pSnapFile->pSst, i);
    item.name = sst;
    item.type = ROCKSDB_SST_TYPE;
    streamGetFileSize(pSnapFile->path, item.name, &item.size);
    taosArrayPush(pSnapFile->pFileList, &item);
  }
  // meta
  item.name = pSnapFile->pCheckpointMeta;
  item.type = ROCKSDB_CHECKPOINT_META_TYPE;
  if (streamGetFileSize(pSnapFile->path, item.name, &item.size) == 0) {
    taosArrayPush(pSnapFile->pFileList, &item);
  }
  return 0;
}
int32_t snapFileReadMeta(SBackendSnapFile2* pSnapFile) {
  TdDirPtr pDir = taosOpenDir(pSnapFile->path);
  if (NULL == pDir) {
    qError("%s failed to open %s", STREAM_STATE_TRANSFER, pSnapFile->path);
    return -1;
  }

  TdDirEntryPtr pDirEntry;
  while ((pDirEntry = taosReadDir(pDir)) != NULL) {
    char* name = taosGetDirEntryName(pDirEntry);
    if (strlen(name) >= strlen(ROCKSDB_CURRENT) && 0 == strncmp(name, ROCKSDB_CURRENT, strlen(ROCKSDB_CURRENT))) {
      pSnapFile->pCurrent = taosStrdup(name);
      continue;
    }
    if (strlen(name) >= strlen(ROCKSDB_MAINFEST) && 0 == strncmp(name, ROCKSDB_MAINFEST, strlen(ROCKSDB_MAINFEST))) {
      pSnapFile->pMainfest = taosStrdup(name);
      continue;
    }
    if (strlen(name) >= strlen(ROCKSDB_OPTIONS) && 0 == strncmp(name, ROCKSDB_OPTIONS, strlen(ROCKSDB_OPTIONS))) {
      pSnapFile->pOptions = taosStrdup(name);
      continue;
    }
    if (strlen(name) >= strlen(ROCKSDB_CHECKPOINT_META) &&
        0 == strncmp(name, ROCKSDB_CHECKPOINT_META, strlen(ROCKSDB_CHECKPOINT_META))) {
      pSnapFile->pCheckpointMeta = taosStrdup(name);
      continue;
    }
    if (strlen(name) >= strlen(ROCKSDB_SST) &&
        0 == strncmp(name + strlen(name) - strlen(ROCKSDB_SST), ROCKSDB_SST, strlen(ROCKSDB_SST))) {
      char* sst = taosStrdup(name);
      taosArrayPush(pSnapFile->pSst, &sst);
    }
  }
  taosCloseDir(&pDir);
  return 0;
}
int32_t streamBackendSnapInitFile(char* path, SStreamTaskSnap* pSnap, SBackendSnapFile2* pSnapFile) {
  // SBanckendFile* pFile = taosMemoryCalloc(1, sizeof(SBanckendFile));
  int32_t code = -1;

  char* snapPath = taosMemoryCalloc(1, strlen(path) + 256);
  sprintf(snapPath, "%s%s%" PRId64 "_%" PRId64 "%s%s%s%s%scheckpoint%" PRId64 "", path, TD_DIRSEP, pSnap->streamId,
          pSnap->taskId, TD_DIRSEP, "state", TD_DIRSEP, "checkpoints", TD_DIRSEP, pSnap->chkpId);
  if (taosIsDir(snapPath)) {
    goto _ERROR;
  }

  pSnapFile->pSst = taosArrayInit(16, sizeof(void*));
  pSnapFile->pFileList = taosArrayInit(64, sizeof(SBackendFileItem));
  pSnapFile->path = snapPath;
  if ((code = snapFileReadMeta(pSnapFile)) != 0) {
    goto _ERROR;
  }
  if ((code = snapFileCvtMeta(pSnapFile)) != 0) {
    goto _ERROR;
  }

  snapFileDebugInfo(pSnapFile);

  code = 0;

_ERROR:
  taosMemoryFree(snapPath);
  return code;
}
void snapFileDestroy(SBackendSnapFile2* pSnap) {
  taosMemoryFree(pSnap->pCheckpointMeta);
  taosMemoryFree(pSnap->pCurrent);
  taosMemoryFree(pSnap->pMainfest);
  taosMemoryFree(pSnap->pOptions);
  taosMemoryFree(pSnap->path);
  for (int i = 0; i < taosArrayGetSize(pSnap->pSst); i++) {
    char* sst = taosArrayGetP(pSnap->pSst, i);
    taosMemoryFree(sst);
  }
  taosArrayDestroy(pSnap->pFileList);
  taosArrayDestroy(pSnap->pSst);
  taosCloseFile(&pSnap->fd);

  return;
}
int32_t streamSnapHandleInit(SStreamSnapHandle* pHandle, char* path, int64_t chkpId, void* pMeta) {
  // impl later

  SArray* pSnapSet = NULL;
  int32_t code = streamBackendGetSnapInfo(pMeta, path, chkpId);
  if (code != 0) {
    return -1;
  }

  SArray* pBdSnapSet = taosArrayInit(8, sizeof(SBackendSnapFile2));

  for (int i = 0; i < taosArrayGetSize(pSnapSet); i++) {
    SStreamTaskSnap* pSnap = taosArrayGet(pSnapSet, i);

    SBackendSnapFile2 snapFile = {0};
    code = streamBackendSnapInitFile(path, pSnap, &snapFile);
    ASSERT(code == 0);
    taosArrayPush(pBdSnapSet, &snapFile);
  }

  pHandle->pBackendSnapSet = pBdSnapSet;
  pHandle->currIdx = 0;
  return 0;

_err:
  streamSnapHandleDestroy(pHandle);

  code = -1;
  return code;
}

void streamSnapHandleDestroy(SStreamSnapHandle* handle) {
  // SBanckendFile* pFile = handle->pBackendFile;
  if (handle->pBackendSnapSet) {
    for (int i = 0; i < taosArrayGetSize(handle->pBackendSnapSet); i++) {
      SBackendSnapFile2* pSnapFile = taosArrayGet(handle->pBackendSnapSet, i);
      snapFileDestroy(pSnapFile);
    }
    taosArrayDestroy(handle->pBackendSnapSet);
  }

  // if (handle->checkpointId == 0) {
  //   // del tmp dir
  //   if (pFile && taosIsDir(pFile->path)) {
  //     taosRemoveDir(pFile->path);
  //   }
  // } else {
  //   streamBackendDelInUseChkp(handle->handle, handle->checkpointId);
  // }
  // if (pFile) {
  //   taosMemoryFree(pFile->pCheckpointMeta);
  //   taosMemoryFree(pFile->pCurrent);
  //   taosMemoryFree(pFile->pMainfest);
  //   taosMemoryFree(pFile->pOptions);
  //   taosMemoryFree(pFile->path);
  //   for (int i = 0; i < taosArrayGetSize(pFile->pSst); i++) {
  //     char* sst = taosArrayGetP(pFile->pSst, i);
  //     taosMemoryFree(sst);
  //   }
  //   taosArrayDestroy(pFile->pSst);
  //   taosMemoryFree(pFile);
  // }
  // taosArrayDestroy(handle->pFileList);
  // taosCloseFile(&handle->fd);
  return;
}

int32_t streamSnapReaderOpen(void* pMeta, int64_t sver, int64_t chkpId, char* path, SStreamSnapReader** ppReader) {
  // impl later
  SStreamSnapReader* pReader = taosMemoryCalloc(1, sizeof(SStreamSnapReader));
  if (pReader == NULL) {
    return TSDB_CODE_OUT_OF_MEMORY;
  }

  if (streamSnapHandleInit(&pReader->handle, (char*)path, chkpId, pMeta) < 0) {
    taosMemoryFree(pReader);
    return -1;
  }

  *ppReader = pReader;

  return 0;
}
int32_t streamSnapReaderClose(SStreamSnapReader* pReader) {
  if (pReader == NULL) return 0;

  streamSnapHandleDestroy(&pReader->handle);
  taosMemoryFree(pReader);
  return 0;
}
int32_t streamSnapRead(SStreamSnapReader* pReader, uint8_t** ppData, int64_t* size) {
  // impl later
  int32_t            code = 0;
  SStreamSnapHandle* pHandle = &pReader->handle;
  SBanckendFile*     pFile = pHandle->pBackendFile;

  SBackendFileItem* item = taosArrayGet(pHandle->pFileList, pHandle->currFileIdx);

  if (pHandle->fd == NULL) {
    if (pHandle->currFileIdx >= taosArrayGetSize(pHandle->pFileList)) {
      // finish
      *ppData = NULL;
      *size = 0;
      return 0;
    } else {
      pHandle->fd = streamOpenFile(pFile->path, item->name, TD_FILE_READ);
      qDebug("%s open file %s, current offset:%" PRId64 ", size:% " PRId64 ", file no.%d", STREAM_STATE_TRANSFER,
             item->name, (int64_t)pHandle->offset, item->size, pHandle->currFileIdx);
    }
  }

  qDebug("%s start to read file %s, current offset:%" PRId64 ", size:%" PRId64 ", file no.%d", STREAM_STATE_TRANSFER,
         item->name, (int64_t)pHandle->offset, item->size, pHandle->currFileIdx);
  uint8_t* buf = taosMemoryCalloc(1, sizeof(SStreamSnapBlockHdr) + kBlockSize);
  int64_t  nread = taosPReadFile(pHandle->fd, buf + sizeof(SStreamSnapBlockHdr), kBlockSize, pHandle->offset);
  if (nread == -1) {
    code = TAOS_SYSTEM_ERROR(terrno);
    qError("%s snap failed to read snap, file name:%s, type:%d,reason:%s", STREAM_STATE_TRANSFER, item->name,
           item->type, tstrerror(code));
    return -1;
  } else if (nread > 0 && nread <= kBlockSize) {
    // left bytes less than kBlockSize
    qDebug("%s read file %s, current offset:%" PRId64 ",size:% " PRId64 ", file no.%d", STREAM_STATE_TRANSFER,
           item->name, (int64_t)pHandle->offset, item->size, pHandle->currFileIdx);
    pHandle->offset += nread;
    if (pHandle->offset >= item->size || nread < kBlockSize) {
      taosCloseFile(&pHandle->fd);
      pHandle->offset = 0;
      pHandle->currFileIdx += 1;
    }
  } else {
    qDebug("%s no data read, close file no.%d, move to next file, open and read", STREAM_STATE_TRANSFER,
           pHandle->currFileIdx);
    taosCloseFile(&pHandle->fd);
    pHandle->offset = 0;
    pHandle->currFileIdx += 1;

    if (pHandle->currFileIdx >= taosArrayGetSize(pHandle->pFileList)) {
      // finish
      *ppData = NULL;
      *size = 0;
      return 0;
    }
    item = taosArrayGet(pHandle->pFileList, pHandle->currFileIdx);
    pHandle->fd = streamOpenFile(pFile->path, item->name, TD_FILE_READ);

    nread = taosPReadFile(pHandle->fd, buf + sizeof(SStreamSnapBlockHdr), kBlockSize, pHandle->offset);
    pHandle->offset += nread;

    qDebug("%s open file and read file %s, current offset:%" PRId64 ", size:% " PRId64 ", file no.%d",
           STREAM_STATE_TRANSFER, item->name, (int64_t)pHandle->offset, item->size, pHandle->currFileIdx);
  }

  SStreamSnapBlockHdr* pHdr = (SStreamSnapBlockHdr*)buf;
  pHdr->size = nread;
  pHdr->type = item->type;
  pHdr->totalSize = item->size;

  memcpy(pHdr->name, item->name, strlen(item->name));
  pHandle->seraial += nread;

  *ppData = buf;
  *size = sizeof(SStreamSnapBlockHdr) + nread;
  return 0;
}
// SMetaSnapWriter ========================================
int32_t streamSnapWriterOpen(void* pMeta, int64_t sver, int64_t ever, char* path, SStreamSnapWriter** ppWriter) {
  // impl later
  SStreamSnapWriter* pWriter = taosMemoryCalloc(1, sizeof(SStreamSnapWriter));
  if (pWriter == NULL) {
    return TSDB_CODE_OUT_OF_MEMORY;
  }
  SStreamSnapHandle* pHandle = &pWriter->handle;

  SBanckendFile* pFile = taosMemoryCalloc(1, sizeof(SBanckendFile));
  pFile->path = taosStrdup(path);
  SArray* list = taosArrayInit(64, sizeof(SBackendFileItem));

  SBackendFileItem item;
  item.name = taosStrdup((char*)ROCKSDB_CURRENT);
  item.type = ROCKSDB_CURRENT_TYPE;
  taosArrayPush(list, &item);

  pHandle->pBackendFile = pFile;

  pHandle->pFileList = list;
  pHandle->currFileIdx = 0;
  pHandle->offset = 0;

  *ppWriter = pWriter;
  return 0;
}

int32_t streamSnapWrite(SStreamSnapWriter* pWriter, uint8_t* pData, uint32_t nData) {
  int32_t code = 0;

  SStreamSnapBlockHdr* pHdr = (SStreamSnapBlockHdr*)pData;
  SStreamSnapHandle*   pHandle = &pWriter->handle;
  SBanckendFile*       pFile = pHandle->pBackendFile;
  SBackendFileItem*    pItem = taosArrayGet(pHandle->pFileList, pHandle->currFileIdx);

  if (pHandle->fd == NULL) {
    pHandle->fd = streamOpenFile(pFile->path, pItem->name, TD_FILE_CREATE | TD_FILE_WRITE | TD_FILE_APPEND);
    if (pHandle->fd == NULL) {
      code = TAOS_SYSTEM_ERROR(terrno);
      qError("%s failed to open file name:%s%s%s, reason:%s", STREAM_STATE_TRANSFER, pFile->path, TD_DIRSEP, pHdr->name,
             tstrerror(code));
    }
  }

  if (strlen(pHdr->name) == strlen(pItem->name) && strcmp(pHdr->name, pItem->name) == 0) {
    int64_t bytes = taosPWriteFile(pHandle->fd, pHdr->data, pHdr->size, pHandle->offset);
    if (bytes != pHdr->size) {
      code = TAOS_SYSTEM_ERROR(terrno);
      qError("%s failed to write snap, file name:%s, reason:%s", STREAM_STATE_TRANSFER, pHdr->name, tstrerror(code));
      return code;
    }
    pHandle->offset += bytes;
  } else {
    taosCloseFile(&pHandle->fd);
    pHandle->offset = 0;
    pHandle->currFileIdx += 1;

    SBackendFileItem item;
    item.name = taosStrdup(pHdr->name);
    item.type = pHdr->type;
    taosArrayPush(pHandle->pFileList, &item);

    SBackendFileItem* pItem = taosArrayGet(pHandle->pFileList, pHandle->currFileIdx);
    pHandle->fd = streamOpenFile(pFile->path, pItem->name, TD_FILE_CREATE | TD_FILE_WRITE | TD_FILE_APPEND);
    if (pHandle->fd == NULL) {
      code = TAOS_SYSTEM_ERROR(terrno);
      qError("%s failed to open file name:%s%s%s, reason:%s", STREAM_STATE_TRANSFER, pFile->path, TD_DIRSEP, pHdr->name,
             tstrerror(code));
    }

    taosPWriteFile(pHandle->fd, pHdr->data, pHdr->size, pHandle->offset);
    pHandle->offset += pHdr->size;
  }

  // impl later
  return 0;
}
int32_t streamSnapWriterClose(SStreamSnapWriter* pWriter, int8_t rollback) {
  SStreamSnapHandle* handle = &pWriter->handle;
  if (qDebugFlag & DEBUG_DEBUG) {
    char* buf = (char*)taosMemoryMalloc(1024);
    int   n = sprintf(buf, "[");
    for (int i = 0; i < taosArrayGetSize(handle->pFileList); i++) {
      SBackendFileItem* item = taosArrayGet(handle->pFileList, i);
      if (i != taosArrayGetSize(handle->pFileList) - 1) {
        n += sprintf(buf + n, "%s %" PRId64 ",", item->name, item->size);
      } else {
        n += sprintf(buf + n, "%s %" PRId64 "]", item->name, item->size);
      }
    }
    qDebug("%s snap get file list, %s", STREAM_STATE_TRANSFER, buf);
    taosMemoryFree(buf);
  }

  for (int i = 0; i < taosArrayGetSize(handle->pFileList); i++) {
    SBackendFileItem* item = taosArrayGet(handle->pFileList, i);
    taosMemoryFree(item->name);
  }

  streamSnapHandleDestroy(handle);
  taosMemoryFree(pWriter);

  return 0;
}
