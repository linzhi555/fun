#include <cassert>
#include <memory>
#include <stdio.h>

template <typename T> struct Result {
  bool err;
  T some;
  const char *errInfo;

public:
  T unwarp() {
    assert(this->err != true && "this result is error,unwarp fail");
    return some;
  };
  bool is_err() { return this->err; }

  static Result Ok(T d) {
    Result res = {.err = false, .some = d, .errInfo = NULL};
    return res;
  };

  static Result Err(const char *e) {
    Result res = {.err = true, .errInfo = e};
    return res;
  };
};

int main() {
  constexpr int n = 11 * 12 * 13;

  printf("%d\n",n);

  Result<int> a = Result<int>::Ok(11);
  printf("%d\n", a.unwarp());

  Result<int> b = Result<int>::Err("fail");
  printf("%d\n", b.unwarp());
}
