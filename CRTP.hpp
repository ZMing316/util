namespace zm
{

template <typename T, template<typename> class crtp_type>
struct CRTP{
    T const& underlying() const {
        return static_cast<T const&>(*this);
    }

    T& underlying(){
        return static_cast<T&>(*this);
    }
};

}

