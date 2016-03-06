// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_CLIPBRD_CLIPBOARD_H_
#define SHINOBU_MODULE_CLIPBRD_CLIPBOARD_H_

#include <windows.h>

#include <map>
#include <memory>
#include <string>

#include "module/clipbrd/ptr_circular_buffer.h"

class Clipboard {
 public:
  class Format {
   public:
    Format(UINT format, SIZE_T size);
    virtual ~Format() {}

    LPVOID Get() const {
      return data_.get();
    }

    template <typename T>
    T& As() const {
      return *reinterpret_cast<T*>(data_.get());
    }

    UINT format() const {
      return format_;
    }

    const std::wstring& name() const {
      return name_;
    }

    SIZE_T size() const {
      return size_;
    }

   private:
    UINT format_;
    std::wstring name_;
    SIZE_T size_;
    std::unique_ptr<char[]> data_;
  };

  class MetaFileFormat : public Format {
   public:
    explicit MetaFileFormat(HENHMETAFILE meta_file);
    virtual ~MetaFileFormat();
  };

  typedef std::map<UINT, std::unique_ptr<Format>> FormatMap;
  typedef FormatMap::const_iterator Iterator;

  explicit Clipboard(HWND owner);

  static std::unique_ptr<Clipboard> Capture();
  static bool Set(const Clipboard& clipboard, HWND fallback_owner);

  bool Has(UINT format_id) const;
  const Format& Get(UINT format_id) const;
  void Add(std::unique_ptr<Format>&& format);  // NOLINT(build/c++11)

  bool IsEmpty() const {
    return formats_.empty();
  }

  HWND owner() const {
    return owner_;
  }

  Iterator begin() const {
    return formats_.begin();
  }

  Iterator end() const {
    return formats_.end();
  }

 private:
  HWND owner_;
  FormatMap formats_;
};

typedef ptr_circular_buffer<Clipboard> ClipboardRing;
typedef ptr_circular_buffer<ClipboardRing> ClipboardPool;

#endif  // SHINOBU_MODULE_CLIPBRD_CLIPBOARD_H_
