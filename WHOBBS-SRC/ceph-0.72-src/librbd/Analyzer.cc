// written by masixiang

#include "common/ceph_context.h"
#include "common/dout.h"
#include "common/errno.h"
#include <ctime>

#include "librbd/Analyzer.h"

namespace librbd {

  Analyzer::Analyzer(ExtentMap *extent_map_p) 
    : extent_map_p(extent_map_p)
  {}

  void *Analyzer::startAnalyzer(void *arg)
  {
    Analyzer *analyzer = (Analyzer *)arg
    ;
    cout << "startAnalyzer" << std::endl;
    int analyze_time = 0;
    while(true) {
      int sleep_time = INTERVAL - analyze_time;
      if(sleep_time > 0) {
        printf("sleep %d seconds\n", sleep_time);
	sleep(sleep_time);
    }
    else
      printf("no sleep time\n");

      time_t start = std::time(NULL);
      analyzer->handle();
      time_t finish = std::time(NULL);
      analyze_time = finish - start;
    }
    return NULL;
  }

  void Analyzer::add_read_op(AnalyzerOp op) 
  {
    read_op_queue.push(op);
  }

  void Analyzer::add_write_op(AnalyzerOp op) 
  {
    write_op_queue.push(op);
  }

  void Analyzer::handle()
  {
    cout << "handle" << std::endl;

    // create a report and print it
    AnalyzerReport *report = create_report(extent_map_p, read_op_queue, write_op_queue);
    report->print_report();
    
    // analyze the report by WHOBBS Placement Model
    std::map<int, std::list<uint64_t> > placement_map = analyze_placement(report);
  }

  AnalyzerReport* Analyzer::create_report(ExtentMap *extent_map_p, std::queue<AnalyzerOp> read_op_queue,
              std::queue<AnalyzerOp> write_op_queue)
  {
    std::map<uint64_t, uint64_t> read_score_map;
    std::map<uint64_t, uint64_t> write_score_map;
    uint64_t ran_reads = 0;
    uint64_t ran_writes = 0;
    uint64_t seq_reads = 0;
    uint64_t seq_writes = 0;
    uint64_t read_bytes = 0;
    uint64_t write_bytes = 0;

    uint64_t i;
    for(i=0; i < extent_map_p->map_size; i++) {
      read_score_map.insert(std::pair<uint64_t, uint64_t>(i, 0));
      write_score_map.insert(std::pair<uint64_t, uint64_t>(i, 0));
    }

    // creating the report
    for(i=0; i < read_op_queue.size(); i++) {
      AnalyzerOp op = read_op_queue.front();
      read_op_queue.pop();

      bool type = op.get_type();
      uint64_t off = op.get_off();
      uint64_t len = op.get_len();

      // calculate the score of the extent by a certain algorithm
      double score = calculate_score(&op);
      uint64_t extent_id = off / extent_map_p->extent_size;
      if(type == READ_OP)
        read_score_map.at(extent_id) += score;
      else
        write_score_map.at(extent_id) += score;

      // update statistics
      if(type == READ_OP) {
        if(is_sequential(off)) {
	  seq_reads ++;
	} else {
	  ran_reads ++;
	}
	read_bytes += len;
      } else {
        if(is_sequential(off)) {
	  seq_writes ++;
	} else {
	  ran_writes ++;
	}
	write_bytes += len;
      }
      set_last_byte(off + len);
    }

    AnalyzerReport *report = new AnalyzerReport(read_score_map, write_score_map, ran_reads, ran_writes,
    	seq_reads, seq_writes, read_bytes, write_bytes);   
    return report;
  }

  double Analyzer::calculate_score(AnalyzerOp *op)
  {
    double score = 0;
    bool type = op->get_type();
    uint64_t off = op->get_off();
    uint64_t len = op->get_len();

    if(is_sequential(off)) {
      score = 0;
    } else {
      if(type == READ_OP) {
        if(len <= SMALL_READ_SIZE) {
          score = 1;
        }
      } else {
        if(len <= SMALL_WRITE_SIZE) {
          score = 1;
        }
      }
    }

    return score;
  }

  bool Analyzer::is_sequential(uint64_t off)
  {
    bool seq = false;
    for(std::list<uint64_t>::iterator it = last_byte_list.begin(); it != last_byte_list.end(); it++) {
      uint64_t distance = 0;
      uint64_t last_byte = *it;
      
      if (off >= last_byte)
        distance = off - last_byte;
      else
        distance = last_byte - off;
      
      if( distance <= SEQ_DISTANCE ) {
        seq = true;
	break;
      }
    }
    return seq;
  }

  void Analyzer::set_last_byte(uint64_t byte)
  {
    int list_size = last_byte_list.size();
    if(list_size < LAST_BYTE_LIST_SIZE)
      last_byte_list.push_back(byte);
    else {
      last_byte_list.pop_front();
      last_byte_list.push_back(byte);
     }
  }

  std::map<int, std::list<uint64_t> > Analyzer::analyze_placement(AnalyzerReport *report)
  {
    std::map<int, std::list<uint64_t> > placement_map;
    return placement_map;
  }

  //--- AnalyzerReport ---//

  AnalyzerReport::AnalyzerReport()
  {}

  AnalyzerReport::AnalyzerReport(std::map<uint64_t, uint64_t> read_score_map, std::map<uint64_t, uint64_t> write_score_map, uint64_t ran_reads, uint64_t ran_writes, uint64_t seq_reads, uint64_t seq_writes, uint64_t read_bytes, uint64_t write_bytes) :
  	read_score_map(read_score_map), write_score_map(write_score_map), ran_reads(ran_reads), ran_writes(ran_writes), seq_reads(seq_reads), seq_writes(seq_writes), read_bytes(read_bytes), write_bytes(write_bytes)
  {}

  void AnalyzerReport::print_report()
  {
  }
  
  //--- AnalyzerOp ---//
  AnalyzerOp::AnalyzerOp(bool type, time_t time, uint64_t off, uint64_t len) :
    time(time), off(off), len(len)
  {}

  bool AnalyzerOp::get_type()
  {
    return type;
  }

  uint64_t AnalyzerOp::get_off()
  {
    return off;
  }
  
  uint64_t AnalyzerOp::get_len()
  {
    return len;
  }

}
