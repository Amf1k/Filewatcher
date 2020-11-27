#include <Windows.h>

#include <array>
#include <iostream>

#include "detail/directorywatcherimpl.hpp"

void fwlib::detail::DirectoryWatcherImpl::startWatching(std::string_view path) {
  HANDLE dir = ::CreateFile(
      path.data(), FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
      OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

  if (dir == INVALID_HANDLE_VALUE) {
    CloseHandle(dir);
    DWORD error = ::GetLastError();
    std::cerr << "Call to CreateFile failed! " << error << "\n";
    std::cerr << "Failed to open directory for change notifications!"
              << std::endl;
    return;
  }

  HANDLE iocp = ::CreateIoCompletionPort(dir, NULL, NULL, 0);

  while (_watchingFlag.load()) {
    std::array<char, 1024 * 8> buf;
    DWORD bytes_read;
    OVERLAPPED overlapped;
    BOOL result = ::ReadDirectoryChangesExW(
        dir, &buf.front(), buf.size(), false,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
        &bytes_read, &overlapped, NULL, ReadDirectoryNotifyExtendedInformation);

    if (result == FALSE) {
      DWORD error = ::GetLastError();
      std::cerr << "Call to ReadDirectoryChangesW failed! " << error << "\n";
      return;
    }

    ULONG_PTR key;
    LPOVERLAPPED overlapped_result;

    if (result = ::GetQueuedCompletionStatus(iocp, &bytes_read, &key,
                                             &overlapped_result, 100);
        result == FALSE) {
      if (DWORD error = ::GetLastError(); error == WAIT_TIMEOUT) {
        continue;
      }
      std::cerr << "Call to GetQueuedCompletionStatus failed!\n";
      return;
    }

    for (FILE_NOTIFY_EXTENDED_INFORMATION* fni =
             reinterpret_cast<FILE_NOTIFY_EXTENDED_INFORMATION*>(&buf.front());
         ; fni = reinterpret_cast<FILE_NOTIFY_EXTENDED_INFORMATION*>(
               reinterpret_cast<char*>(fni) + fni->NextEntryOffset)) {
      std::array<char, MAX_PATH> fileName;
      int count = WideCharToMultiByte(
          CP_ACP, 0, fni->FileName, fni->FileNameLength / sizeof(WCHAR),
          &fileName.front(), fileName.size(), NULL, NULL);
      fileName[count] = '\0';

      FileAction fwAction;

      switch (fni->Action) {
        case FILE_ACTION_RENAMED_NEW_NAME:
        case FILE_ACTION_ADDED:
          fwAction = FileAction::Create;
          break;
        case FILE_ACTION_RENAMED_OLD_NAME:
        case FILE_ACTION_REMOVED:
          fwAction = FileAction::Delete;
          break;
        case FILE_ACTION_MODIFIED:
          fwAction = FileAction::Modify;
          break;
      };
      notify(FileInfo{fileName.data(),
                      // FIXME: Вот тут какая то дичь началась, не удалось
                      // нормально время пересчитать, бах-баг!
                      static_cast<std::uintmax_t>(fni->FileSize.QuadPart),
                      std::filesystem::file_time_type(
                          std::chrono::duration<std::int64_t>(
                              fni->LastModificationTime.QuadPart))},
             fwAction);
      if (fni->NextEntryOffset == 0)
        break;
    }
  }
  CloseHandle(dir);
  CloseHandle(iocp);
}
