#ifndef XFCE_APPLET_SPEED_RTTI_H
#define XFCE_APPLET_SPEED_RTTI_H

// Static cast
template <typename DestTy,
          typename SourceTy,
          typename std::enable_if_t<std::is_class<SourceTy>::value &&
                                        std::is_class<DestTy>::value,
                                    int> = 0>
DestTy& cast(SourceTy& src) {
  return *static_cast<DestTy*>(&src);
}

template <typename DestTy,
          typename SourceTy,
          typename std::enable_if_t<std::is_class<SourceTy>::value &&
                                        std::is_class<DestTy>::value,
                                    int> = 0>
const DestTy& cast(const SourceTy& src) {
  return *static_cast<DestTy*>(&src);
}

template <typename DestTy,
          typename SourceTy,
          typename std::enable_if_t<std::is_class<SourceTy>::value &&
                                        std::is_class<DestTy>::value,
                                    int> = 0>
DestTy* cast(SourceTy* src) {
  return static_cast<DestTy*>(src);
}

template <typename DestTy,
          typename SourceTy,
          typename std::enable_if_t<std::is_class<SourceTy>::value &&
                                        std::is_class<DestTy>::value,
                                    int> = 0>
const DestTy* cast(const SourceTy* src) {
  return static_cast<DestTy*>(src);
}

// Dynamic cast
template <typename DestTy,
          typename SourceTy,
          typename std::enable_if_t<std::is_class<SourceTy>::value &&
                                        std::is_class<DestTy>::value,
                                    int> = 0>
DestTy& dyn_cast(SourceTy& src) {
  if(DestTy::classof(&src))
    return *static_cast<DestTy*>(&src);
  return *((DestTy*)nullptr);
}

template <typename DestTy,
          typename SourceTy,
          typename std::enable_if_t<std::is_class<SourceTy>::value &&
                                        std::is_class<DestTy>::value,
                                    int> = 0>
const DestTy& dyn_cast(const SourceTy& src) {
  if(DestTy::classof(&src))
    return *static_cast<const DestTy*>(&src);
  return *((const DestTy*)nullptr);
}

template <typename DestTy,
          typename SourceTy,
          typename std::enable_if_t<std::is_class<SourceTy>::value &&
                                        std::is_class<DestTy>::value,
                                    int> = 0>
DestTy* dyn_cast(SourceTy* src) {
  if(DestTy::classof(src))
    return static_cast<DestTy*>(src);
  return nullptr;
}

template <typename DestTy,
          typename SourceTy,
          typename std::enable_if_t<std::is_class<SourceTy>::value &&
                                        std::is_class<DestTy>::value,
                                    int> = 0>
const DestTy* dyn_cast(const SourceTy* src) {
  if(DestTy::classof(src))
    return static_cast<const DestTy*>(src);
  return nullptr;
}

// Type check
template <typename DestTy,
          typename SourceTy,
          typename std::enable_if_t<std::is_class<SourceTy>::value &&
                                        std::is_class<DestTy>::value,
                                    int> = 0>
bool isa(const SourceTy* src) {
  return DestTy::classof(src);
}

template <typename DestTy,
          typename SourceTy,
          typename std::enable_if_t<std::is_class<SourceTy>::value &&
                                        std::is_class<DestTy>::value,
                                    int> = 0>
bool isa(const SourceTy& src) {
  return DestTy::classof(&src);
}

#endif // XFCE_APPLET_SPEED_RTTI_H
