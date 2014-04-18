// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
#include <map>
#include <set>
#include <string>
#include <tr1/memory>

#include "os/KeyValueDB.h"
#include "include/buffer.h"
#include "include/Context.h"

using std::string;

class KeyValueDBMemory : public KeyValueDB {
public:
  std::map<std::pair<string,string>,bufferlist> db;

  KeyValueDBMemory() { }
  KeyValueDBMemory(KeyValueDBMemory *db) : db(db->db) { }
  virtual ~KeyValueDBMemory() { }

  int get(
    const string &prefix,
    const std::set<string> &key,
    std::map<string, bufferlist> *out
    );

  int get_keys(
    const string &prefix,
    const std::set<string> &key,
    std::set<string> *out
    );

  int set(
    const string &prefix,
    const string &key,
    const bufferlist &bl
    );

  int rmkey(
    const string &prefix,
    const string &key
    );

  int rmkeys_by_prefix(
    const string &prefix
    );

  class TransactionImpl_ : public TransactionImpl {
  public:
    list<Context *> on_commit;
    KeyValueDBMemory *db;

    TransactionImpl_(KeyValueDBMemory *db) : db(db) {}


    struct SetOp : public Context {
      KeyValueDBMemory *db;
      std::pair<string,string> key;
      bufferlist value;
      SetOp(KeyValueDBMemory *db,
	    const std::pair<string,string> &key,
	    const bufferlist &value)
	: db(db), key(key), value(value) {}
      void finish(int r) {
	db->set(key.first, key.second, value);
      }
    };

    void set(const string &prefix, const string &k, const bufferlist& bl) {
      on_commit.push_back(new SetOp(db, std::make_pair(prefix, k), bl));
    }

    struct RmKeysOp : public Context {
      KeyValueDBMemory *db;
      std::pair<string,string> key;
      RmKeysOp(KeyValueDBMemory *db,
	       const std::pair<string,string> &key)
	: db(db), key(key) {}
      void finish(int r) {
	db->rmkey(key.first, key.second);
      }
    };

    void rmkey(const string &prefix, const string &key) {
      on_commit.push_back(new RmKeysOp(db, std::make_pair(prefix, key)));
    }

    struct RmKeysByPrefixOp : public Context {
      KeyValueDBMemory *db;
      string prefix;
      RmKeysByPrefixOp(KeyValueDBMemory *db,
		       const string &prefix)
	: db(db), prefix(prefix) {}
      void finish(int r) {
	db->rmkeys_by_prefix(prefix);
      }
    };
    void rmkeys_by_prefix(const string &prefix) {
      on_commit.push_back(new RmKeysByPrefixOp(db, prefix));
    }

    int complete() {
      for (list<Context *>::iterator i = on_commit.begin();
	   i != on_commit.end();
	   on_commit.erase(i++)) {
	(*i)->finish(0);
	delete *i;
      }
      return 0;
    }

    ~TransactionImpl_() {
      for (list<Context *>::iterator i = on_commit.begin();
	   i != on_commit.end();
	   on_commit.erase(i++)) {
	delete *i;
      }
    }
  };

  Transaction get_transaction() {
    return Transaction(new TransactionImpl_(this));
  }

  int submit_transaction(Transaction trans) {
    return static_cast<TransactionImpl_*>(trans.get())->complete();
  }

private:
  bool exists_prefix(const string &prefix) {
    std::map<std::pair<string,string>,bufferlist>::iterator it;
    it = db.lower_bound(std::make_pair(prefix, ""));
    return ((it != db.end()) && ((*it).first.first == prefix));
  }

  friend class WholeSpaceMemIterator;

protected:
  WholeSpaceIterator _get_iterator();
  WholeSpaceIterator _get_snapshot_iterator();
};
