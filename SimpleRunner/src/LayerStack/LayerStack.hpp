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
    virtual void Update() = 0;
    virtual void Render() = 0;

    TypeID id;
};

class LayerStack
{
public:
    template<typename TLayer = Layer, typename...TArgs>
    void AddLayer(TArgs&&... args)
    {
        const auto& found = std::find_if(
            m_layers.begin(), m_layers.end(),
            [&](const auto& layer)
            {
                return layer->id == GetTypeID<TLayer>();
            }
        );

        if(found != m_layers.end())
        {
            std::unique_ptr<TLayer> layer(std::forward<TArgs>(args)...);
            layer->id = GetTypeID<TLayer>();
            m_layers.emplace_back(std::move(layer));
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
