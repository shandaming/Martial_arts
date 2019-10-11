/*
 * Copyright(C) 2018
 */

#ifndef COMMON_PTR_VECTOR_H
#define COMMON_PTR_VECTOR_H

template<typename Type>
class Ptr_vector
{
public:
    typedef std::vector<Type*> Vector;

    Ptr_vector () {}

    ~Ptr_vector ()
    {
        clear ();
    }

    Vector* operator->()
    {
        return &vector_;
    }

    const Vector* operator->() const
    {
        return &vector_;
    }

    Vector& operator*()
    {
        return vector_;
    }

    const Vector& operator*() const
    {
        return vector_;
    }

    Type*& operator[](const size_t index_)
    {
        return vector_[index_];
    }

    Type* const& operator[](const size_t index_) const
    {
        return vector_[index_];
    }

    bool operator==(const Ptr_vector& rhs_) const
    {
        bool equal_ = vector_.size () == rhs_.vector_.size ();

        if (equal_)
        {
            typename Vector::const_iterator lhs_iter_ = vector_.begin ();
            typename Vector::const_iterator end_ = vector_.end ();
            typename Vector::const_iterator rhs_iter_ = rhs_.vector_.begin ();

            for (; equal_ && lhs_iter_ != end_; ++lhs_iter_, ++rhs_iter_)
            {
                equal_ = **lhs_iter_ == **rhs_iter_;
            }
        }

        return  equal_;
    }

    void clear ()
    {
        if (!vector_.empty())
        {
            Type **iter_ = &vector_.front ();
            Type **end_ = iter_ + vector_.size ();

            for (; iter_ != end_; ++iter_)
            {
                delete *iter_;
            }
        }

        vector_.clear();
    }

private:
    Vector vector_;

    Ptr_vector(const Ptr_vector&) = delete;
    Ptr_vector& operator = (const Ptr_vector&) = delete;
};

#endif
