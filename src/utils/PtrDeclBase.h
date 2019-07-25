//
// Created by konnod on 11/20/18.
//

#ifndef THREADING_PTRDECLBASE_H
#define THREADING_PTRDECLBASE_H

#include <memory>

/**
 * Convenient base class that offers smart pointers declaration types for derived class
 * You should not derive from PtrDeclBase if derived the class is going to be base class,
 * because it will pollute the derived class namespace and
 * will cause these types to be pointers to that derived class for any class that will
 * derive from it
 */
template <class D>
class PtrDeclBase {
public:
    using SPtr = std::shared_ptr<D>;
    using UPtr = std::unique_ptr<D>;
    using WPtr = std::weak_ptr<D>;
};

#endif //LTHREADING_PTRDECLBASE_H
