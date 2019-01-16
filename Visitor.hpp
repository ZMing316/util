#pragma once

namespace zm
{
struct VisitorToken
{
    virtual ~VisitorToken() = default;
};

template <typename T>
struct Visitor
{
    virtual ~Visitor() = default;
    virtual void visit(T *ele) = 0;
};

template <typename... T>
struct MultiVisitor : VisitorToken, Visitor<T>...
{
    using Visitor<T>::visit...;
};

template <typename T>
struct Visitable
{
    virtual ~Visitable() = default;
    virtual void accept(VisitorToken *visitor)
    {
        if (auto c_visitor = dynamic_cast<Visitor<T> *>(visitor); c_visitor)
        {
            c_visitor->visit(static_cast<T *>(this));
        }
    }
};

//  struct Animal;
//  struct Dog : Animal, Visitable<Dog>; 
//  struct Fish : Animal, Visitable<Fish>;
//  struct MyVisitor : MuliVisitor<Dog, Fish>;

} // namespace zm