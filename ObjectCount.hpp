#include <atomic>
#include <cstdint>

namespace zm
{
template <typename T>
class ObjectCount{
    protected:
        ObjectCount(){
            ObjectCount<T>::count_.fetch_add(1);
        }

        ~ObjectCount(){
            ObjectCount<T>::count_.fetch_sub(1);
        }
        
        ObjectCount(const ObjectCount&){
            ObjectCount<T>::count_.fetch_add(1);
        }
    public:
        static size_t count() {
            return ObjectCount<T>::count_.load();
        }

    private:
        static std::atomic<uint64_t> count_;
};

template <typename T>
std::atomic<uint64_t> ObjectCount<T>::count_{0};
}
