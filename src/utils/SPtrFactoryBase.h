//
// Created by konnod on 11/16/18.
//

#ifndef THREADING_SPTRFACTORYBASE_H
#define THREADING_SPTRFACTORYBASE_H

#include <memory>
#include "PtrDeclBase.h"

/**
 * Provides convenient member function to create unique pointer to object of class D.
 * D can be the derived class name
 * @tparam D Derived class type
 */
template <class D>
class SPtrFactoryBase : public PtrDeclBase<D> {
public:
    template<typename... Args>
    static typename PtrDeclBase<D>::UPtr create(Args&&... args)
    { return std::make_unique<D>(std::forward<Args>(args)...); }
};

#endif //THREADING_SPTRFACTORYBASE_H
