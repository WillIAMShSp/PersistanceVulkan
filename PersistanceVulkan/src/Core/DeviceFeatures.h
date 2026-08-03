#pragma once
#include "../PersistanceLib.h"
#include <vector>
#include <memory>

namespace PersistanceUtils
{

   

    
    template<class T>
    class DeviceFeatures 
    {   
        public:
        DeviceFeatures(T feature = {}): m_baseFeature(std::make_unique<T>(feature)) 
        {

            m_tail = &m_baseFeature->pNext;

        };

        template<typename C> 
        void addFeature(C& feature) {
            *m_tail = &feature;
            m_tail = &feature.pNext;

        }

        T* getFeatures() {
            return m_baseFeature.get();
        }

        private:

        std::unique_ptr<T> m_baseFeature;
        void** m_tail = nullptr;
    

    };

}