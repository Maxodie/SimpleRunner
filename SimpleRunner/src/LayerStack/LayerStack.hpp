#pragma once
#include "Core/Core.hpp"
#include "Event/Event.hpp"

namespace SR
{

class Layer
{
public:
    Layer() = default;
    virtual ~Layer() = default;
    virtual void OnEvent(Event& event) = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;

    TypeIDptr id;
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
                return layer->id == GetTypeIDptr<TLayer>();
            }
        );

        if(found == m_layers.end())
        {
            std::unique_ptr<TLayer> layer = std::make_unique<TLayer>(std::forward<TArgs>(args)...);
            layer->id = GetTypeIDptr<TLayer>();
            CORE_LOG_SUCCESS("Layer %s has been added to layer stack", layer->id->name());
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
                    if(layer->id == GetTypeIDptr<TLayer>())
                    {
                        CORE_LOG_SUCCESS("Layer has been removed from layer stack");
                        return true;
                    }

                    return false;
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
