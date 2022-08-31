/*
 * @Descripttion: 
 * @version: 
 * @Author: xuboluo
 * @Date: 2022-08-03 16:24:10
 * @LastEditors: xuboluo
 * @LastEditTime: 2022-08-23 17:03:47
 */
#pragma once
#include <raft-kv/raft/proto.h>

namespace kv {

// Unstable.entries[i] has raft log position i+unstable.offset.
// Note that unstable.offset may be less than the highest log
// position in storage; this means that the next write to storage
// might need to truncate the log before persisting unstable.entries.
// Unstable 用于保存还没有被用户层持久化的数据
class Unstable {
 public:
  explicit Unstable(uint64_t offset)
      : offset_(offset) {

  }

  // maybe_first_index returns the index of the first possible entry in entries
  // if it has a snapshot.
  void maybe_first_index(uint64_t& index, bool& ok);

  // maybe_last_index returns the last index if it has at least one
  // unstable entry or snapshot.
  void maybe_last_index(uint64_t& index, bool& ok);

  // maybe_term returns the term of the entry at index i, if there
  // is any.
  // 根据 i 得到 index 
  void maybe_term(uint64_t index, uint64_t& term, bool& ok);

  void stable_to(uint64_t index, uint64_t term);

  void stable_snap_to(uint64_t index);

  void restore(proto::SnapshotPtr snapshot);

  void truncate_and_append(std::vector<proto::EntryPtr> entries); // 将 msg->entry 的数据添加到 unstable 的 entry 中

  void slice(uint64_t low, uint64_t high, std::vector<proto::EntryPtr>& entries);
 public:
  // the incoming unstable snapshot, if any.
  proto::SnapshotPtr snapshot_;

  // all entries that have not yet been written to storage.
  std::vector<proto::EntryPtr> entries_;
  // 保存 snap 中最后一条 entry 的索引
  uint64_t offset_; 
};
typedef std::shared_ptr<Unstable> UnstablePtr;

}
