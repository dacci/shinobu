// Copyright (c) 2016 dacci.org

#include "module/clipbrd/clipboard.h"

Clipboard::Format::Format(UINT format, SIZE_T size)
    : format_(format), size_(size), data_(std::make_unique<char[]>(size)) {
  if (0xC000 <= format) {  // registered format
    name_.resize(256);
    auto length = GetClipboardFormatNameW(format, &name_[0],
                                          static_cast<int>(name_.size() + 1));
    name_.resize(length);
  }
}

Clipboard::MetaFileFormat::MetaFileFormat(HENHMETAFILE meta_file)
    : Format(CF_ENHMETAFILE, sizeof(HENHMETAFILE)) {
  auto& handle = As<HENHMETAFILE>();
  handle = meta_file;
}

Clipboard::MetaFileFormat::~MetaFileFormat() {
  DeleteEnhMetaFile(As<HENHMETAFILE>());
}

Clipboard::Clipboard(HWND owner) : owner_(owner) {}

std::unique_ptr<Clipboard> Clipboard::Capture() {
  auto clipboard = std::make_unique<Clipboard>(GetClipboardOwner());
  if (clipboard == nullptr)
    return nullptr;

  UINT format_id = 0;
  for (auto i = 0, l = CountClipboardFormats(); i < l; ++i) {
    format_id = EnumClipboardFormats(format_id);
    if (format_id == 0)  // error
      break;

    auto handle = GetClipboardData(format_id);
    if (handle == NULL)  // error
      continue;

    std::unique_ptr<Format> format;

    if (format_id == CF_BITMAP) {
      // ignore

    } else if (format_id == CF_ENHMETAFILE) {
      auto meta_file =
          CopyEnhMetaFile(reinterpret_cast<HENHMETAFILE>(handle), nullptr);
      if (meta_file == NULL)  // error
        continue;

      format = std::make_unique<MetaFileFormat>(meta_file);

    } else {
      auto size = GlobalSize(handle);
      auto memory = GlobalLock(handle);
      if (memory == nullptr)  // error
        continue;

      format = std::make_unique<Format>(format_id, size);
      memcpy(format->Get(), memory, size);

      GlobalUnlock(handle);
    }

    if (format != nullptr)
      clipboard->Add(std::move(format));
  }

  if (clipboard->IsEmpty())
    return nullptr;

  return clipboard;
}

bool Clipboard::Set(const Clipboard& clipboard, HWND fallback_owner) {
  auto fallback = false;
  if (!OpenClipboard(clipboard.owner())) {
    if (!OpenClipboard(fallback_owner))
      return false;

    fallback = true;
  }

  if (!EmptyClipboard()) {
    CloseClipboard();
    return false;
  }

  for (auto& pair : clipboard) {
    const auto& format_id = pair.first;
    if (fallback && format_id == CF_OWNERDISPLAY)
      continue;

    auto& format = pair.second;

    auto handle = GlobalAlloc(GMEM_MOVEABLE, format->size());
    if (handle == NULL)
      continue;

    auto failed = true;
    auto memory = GlobalLock(handle);
    if (memory != nullptr) {
      memcpy(memory, format->Get(), format->size());
      GlobalUnlock(memory);

      if (SetClipboardData(format_id, handle) != NULL)
        failed = false;
    }

    if (failed)
      GlobalFree(handle);
  }

  CloseClipboard();

  return true;
}

bool Clipboard::Has(UINT format_id) const {
  return formats_.find(format_id) != formats_.end();
}

const Clipboard::Format& Clipboard::Get(UINT format_id) const {
  return *formats_.at(format_id);
}

void Clipboard::Add(std::unique_ptr<Format>&& format) {  // NOLINT(build/c++11)
  auto format_id = format->format();
  formats_.insert({format_id, std::move(format)});
}
