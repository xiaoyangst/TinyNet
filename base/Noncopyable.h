#ifndef TINYNET_BASE_NONCOPYABLE_H_
#define TINYNET_BASE_NONCOPYABLE_H_
namespace xy {
class Noncopyable {
 protected:
  Noncopyable() = default;
  ~Noncopyable() = default;

 public:
  // 禁用拷贝构造和赋值，派生类将无法被拷贝
  Noncopyable(const Noncopyable &) = delete;
  Noncopyable &operator=(const Noncopyable &) = delete;
};
}  // namespace xy
#endif //TINYNET_BASE_NONCOPYABLE_H_

/******** Noncopyable 作用
 * Noncopyable 类用于禁止派生类对象的拷贝构造和拷贝赋值操作。
 * 通过继承 Noncopyable，派生类会自动禁用拷贝构造函数和拷贝赋值运算符，从而防止对象被复制。
 * 这在需要确保对象唯一性或防止资源管理错误（如内存泄漏或双重释放）时非常有用。
 * 例如，在包含动态内存分配的类中，禁止拷贝操作可以避免在拷贝时发生资源管理错误。
 */