#pragma once
#include <stdint.h>
#include <vector>
#include <msgpack.hpp>

namespace kv {

namespace proto {

typedef uint8_t MessageType;

const MessageType MsgHup = 0; // 自己发送给自己的一个消息告诉自己可以转变为 cand statue 然后开始选举
const MessageType MsgBeat = 1; // leader 向其他节点发送心跳
const MessageType MsgProp = 2; // follower 向 leader 发送消息
const MessageType MsgApp = 3; // leader 向 follower 发送消息
const MessageType MsgAppResp = 4;
const MessageType MsgVote = 5;
const MessageType MsgVoteResp = 6;
const MessageType MsgSnap = 7;
const MessageType MsgHeartbeat = 8;
const MessageType MsgHeartbeatResp = 9;
const MessageType MsgUnreachable = 10;
const MessageType MsgSnapStatus = 11;
const MessageType MsgCheckQuorum = 12; // 这个消息是leader发送给follower的消息，让follower告诉leader自己是否存活
const MessageType MsgTransferLeader = 13;
const MessageType MsgTimeoutNow = 14;
const MessageType MsgReadIndex = 15;
const MessageType MsgReadIndexResp = 16;
const MessageType MsgPreVote = 17;
const MessageType MsgPreVoteResp = 18;

const MessageType MsgTypeSize = 19;

const char* msg_type_to_string(MessageType type);

typedef uint8_t EntryType;

const EntryType EntryNormal = 0; // 普通的日志
const EntryType EntryConfChange = 1; // 集群节点发生变化发送的日志

const char* entry_type_to_string(EntryType type);

// 每一个对象都会创一个 Entry
struct Entry {
  Entry()
      : type(EntryNormal),
        term(0),
        index(0) {}

  explicit Entry(Entry&& entry)
      : type(entry.type),  // entry 类型
        term(entry.term), // leader term
        index(entry.index), // index 
        data(std::move(entry.data)) { // 指令


  }

  kv::proto::Entry& operator=(const kv::proto::Entry& entry) = default;
  Entry(const Entry& entry) = default;

  explicit Entry(EntryType type, uint64_t term, uint64_t index, std::vector<uint8_t> data)
      : type(type),
        term(term),
        index(index),
        data(std::move(data)) {}

  uint32_t serialize_size() const;

  uint32_t payload_size() const {
    return static_cast<uint32_t>(data.size());
  }

  bool operator==(const Entry& entry) const {
    return type == entry.type && term == entry.term && index == entry.index && data == entry.data;
  }
  bool operator!=(const Entry& entry) const {
    return !(*this == entry);
  }

  EntryType type;
  uint64_t term;
  uint64_t index;
  std::vector<uint8_t> data;
  MSGPACK_DEFINE (type, term, index, data);
};
typedef std::shared_ptr<Entry> EntryPtr;

struct ConfState {
  bool operator==(const ConfState& cs) const {
    return nodes == cs.nodes && learners == cs.learners;
  }

  std::vector<uint64_t> nodes;
  std::vector<uint64_t> learners;
  MSGPACK_DEFINE (nodes, learners);
};
typedef std::shared_ptr<ConfState> ConfStatePtr;

struct SnapshotMetadata {
  SnapshotMetadata()
      : index(0),
        term(0) {
  }

  bool operator==(const SnapshotMetadata& meta) const {
    return conf_state == meta.conf_state && index == meta.index && term == meta.term;
  }

  ConfState conf_state;
  uint64_t index;
  uint64_t term;
  MSGPACK_DEFINE (conf_state, index, term);
};

struct Snapshot {
  Snapshot() = default;

  explicit Snapshot(const std::vector<uint8_t>& data)
      : data(data) {
  }

  bool equal(const Snapshot& snap) const;

  bool is_empty() const {
    return metadata.index == 0;
  }
  std::vector<uint8_t> data;
  SnapshotMetadata metadata;
  MSGPACK_DEFINE (data, metadata); // 将 data 对象序列化 metadata 对象
};
typedef std::shared_ptr<Snapshot> SnapshotPtr;

// 消息的抽象
struct Message {
  Message()
      : type(MsgHup), // 消息的类型
        to(0), // 目标 id 
        from(0), // orign id 
        term(0), // orign 的 item
        log_term(0),  // 该消息携带的第一条Entry记录的Term值 
        index(0), // index 值
        commit(0), // orign 节点的 index 
        reject(false), // 如果 follower 不能收到信息则设置为 true 
        reject_hint(0) { // follower 提供给 leader 的索引值

  }

  bool operator==(const Message& msg) const {
    return type == msg.type && to == msg.to && from == msg.from && term == msg.term
        && log_term == msg.log_term && index == msg.index
        && entries == msg.entries && commit == msg.commit
        && snapshot.equal(msg.snapshot) && reject == msg.reject
        && reject_hint == msg.reject_hint && context == msg.context;
  }

  bool is_local_msg() const;

  bool is_response_msg() const;

  MessageType type;
  uint64_t to;
  uint64_t from;
  uint64_t term;
  uint64_t log_term;
  uint64_t index;
  std::vector<Entry> entries;
  uint64_t commit;
  Snapshot snapshot;
  bool reject;
  uint64_t reject_hint;
  std::vector<uint8_t> context;
  MSGPACK_DEFINE (type, to, from, term, log_term, index, entries, commit, snapshot, reject, reject_hint, context);
};
typedef std::shared_ptr<Message> MessagePtr;

struct HardState {
  HardState()
      : term(0), 
        vote(0), 
        commit(0) { 
  }
  // 判断是否是最原始状态
  bool is_empty_state() const {
    return term == 0 && vote == 0 && commit == 0;
  }

  bool equal(const HardState& hs) const {
    return term == hs.term && vote == hs.vote && commit == hs.commit;
  }

  uint64_t term; // 节点的任期
  uint64_t vote; // 投票
  uint64_t commit; // 已提交日志的信息
  MSGPACK_DEFINE (term, vote, commit);
};

const uint8_t ConfChangeAddNode = 0;
const uint8_t ConfChangeRemoveNode = 1;
const uint8_t ConfChangeUpdateNode = 2;
const uint8_t ConfChangeAddLearnerNode = 3;

struct ConfChange {
  static void from_data(const std::vector<uint8_t>& data, ConfChange& cc);
  uint64_t id;
  uint8_t conf_change_type;
  uint64_t node_id;
  std::vector<uint8_t> context;
  MSGPACK_DEFINE (id, conf_change_type, node_id, context);
  std::vector<uint8_t> serialize() const;
};
typedef std::shared_ptr<ConfChange> ConfChangePtr;

}
}