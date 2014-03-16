// written by masixiang

#include <map>
#include <queue>
#include <string>

#include "librbd/WHOBBS.h"

namespace librbd {

#ifndef ANALYZER
#define ANALYZER

  class AnalyzerOp;
  class AnalyzerReport;

  class Analyzer
  {
  public:
    Analyzer(ExtentMap *extent_map_p);
    static void *startAnalyzer(void *arg);
    void add_op(AnalyzerOp op);

  protected:
    void handle();
    AnalyzerReport* create_report(ExtentMap *extent_map_p, std::queue<AnalyzerOp> *op_queue);
    double calculate_score(AnalyzerOp *op);
    bool is_sequential(uint64_t off);
    void set_last_byte(uint64_t byte);
    std::map<int, std::list<uint64_t> > analyze_placement(AnalyzerReport *report);

  private:
    ExtentMap *extent_map_p;
    std::queue<AnalyzerOp> op_queue;
    std::list<uint64_t> last_byte_list;
  };

  class AnalyzerReport
  {
  public:
    AnalyzerReport();
    AnalyzerReport(ExtentMap *extent_map_p, std::map<uint64_t, uint64_t> read_score_map, std::map<uint64_t, uint64_t> write_score_map, uint64_t ran_reads, uint64_t ran_writes, uint64_t seq_reads, uint64_t seq_writes, uint64_t read_bytes, uint64_t write_bytes);
    void print_report();
    std::list<uint64_t> tend_to(int pool);

  private:
    ExtentMap *extent_map_p;
    std::map<uint64_t, uint64_t> read_score_map;
    std::map<uint64_t, uint64_t> write_score_map;
    uint64_t ran_reads;
    uint64_t ran_writes;
    uint64_t seq_reads;
    uint64_t seq_writes;
    uint64_t read_bytes;
    uint64_t write_bytes;
  };

  class AnalyzerOp
  {
  public:
    AnalyzerOp(bool type, time_t time, uint64_t off, uint64_t len);
    bool get_type();
    uint64_t get_off();
    uint64_t get_len();

  private:
    bool type;
    time_t time;
    uint64_t off;
    uint64_t len;
  };
#endif
}
