namespace librbd {

  class Analyzer
  {
  public:
    static void inc_num();
    static void print_num();

  private:
    static int num;
  };
}
