#pragma once
#include <vector>
#include <string>
#include <utility>
#include "device_info.h"
#include "device/data_store.h"
#include "op/physical.h"
#include "op/physical_fn.h"
#include "procedures/device_listener.h"
#include "common/common.h"
#include "common/concurrent_blocking_queue.h"

namespace minerva {

class Device {
 public:
  enum class MemType {
    kCpu,
    kGpu
  };
  Device(uint64_t, DeviceListener*);
  virtual ~Device();
  virtual void PushTask(uint64_t) = 0;
  virtual std::pair<MemType, float*> GetPtr(uint64_t);
  virtual std::string Name() const;

 protected:
  std::unordered_set<uint64_t> local_data_;
  std::unordered_set<uint64_t> remote_data_;
  uint64_t device_id_;
  DataStore* data_store_;
  DeviceListener* listener_;

 private:
  Device();
  DISALLOW_COPY_AND_ASSIGN(Device);
};

#ifdef HAS_CUDA

class GpuDevice : public Device {
 public:
  GpuDevice(uint64_t, DeviceListener*);
  ~GpuDevice();
  void PushTask(uint64_t);
  std::pair<MemType, float*> GetPtr(uint64_t);
  std::string Name() const;

 private:
  static const size_t kDefaultStreamNum = 16;
  void Execute(uint64_t);
  cudaStream_t GetSomeStream();
  cudaStream_t stream_[kDefaultStreamNum];
  ThreadPool pool_;
  DISALLOW_COPY_AND_ASSIGN(GpuDevice);
};

#endif

class CpuDevice : public Device {
 public:
  CpuDevice(uint64_t, DeviceListener*);
  ~CpuDevice();
  void PushTask(uint64_t);
  std::pair<MemType, float*> GetPtr(uint64_t);
  std::string Name() const;
 
 private:
  void Execute(uint64_t);
  DISALLOW_COPY_AND_ASSIGN(CpuDevice);
};

}  // namespace minerva

