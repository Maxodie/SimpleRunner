#pragma once
#include "Core/Core.hpp"
#include "Event/Event.hpp"

namespace SR
{

class Layer
{
public:
    virtual ~Layer();
    virtual void OnEvent(Event& event) = 0;

    TypeID id;
};

class LayerStack
{
public:
    template<typename TLayer, typename...TArgs>
    void AddLayer(TArgs&&... args)
    {
        auto& found = std::find_if(
            m_layers.begin(), m_layers.end(),
            [&](auto& layer)
            {
                return layer->id == GetTypeID<TLayer>();
            }
        );

        if(!*found)
        {
            m_layers.emplace_back(std::forward(args)...);
        }
    }

    template<typename TLayer, typename...TArgs>
    void RemoveLayer()
    {
        m_layers.erase(
            std::remove_if(
                m_layers.begin(), m_layers.end(),
                [&](auto& layer)
                {
                    return layer->id == GetTypeID<TLayer>();
                }
            )
        );
    }

    SR_INLINE const std::vector<std::unique_ptr<Layer>>& GetStack()
    {
        return m_layers;
    }

private:
    std::vector<std::unique_ptr<Layer>> m_layers;
};

}
