#pragma once

#include <rk_types.h>

#include "KartObjectProxy.hpp"
#include "KartPhysicsEngine.hpp"
#include "KartSettings.hpp"

#include "kart/KartObjectProxy.hpp"
#include "system/ResourceManager.hpp"

// from kinoko
namespace Kart {

/// @brief The highest level abstraction for a kart.
class KartObject : public KartObjectProxy {
public:
    KartObject(KartParam *param);
    virtual ~KartObject();
    virtual KartBody *createBody(KartPhysics *physics);
    virtual void createTires();

    void init();
    void initImpl();
    void prepare();
    void prepareTiresAndSuspensions();

    void createSub();
    void createModel();

    void calcSub();
    void calc();

    const KartAccessor *accessor() const;

    static KartObject *Create(System::CharacterId character, System::VehicleId vehicle, u8 playerIdx);

protected:
    KartPhysicsEngine *m_physicsEngine;
    KartSettings *m_settings;
    void *m_model;
    KartAccessor m_pointers;
};
static_assert(sizeof(KartObject) == 0x80);

/// @brief The highest level abstraction for a bike.
class KartObjectBike : public KartObject {
public:
    KartObjectBike(KartParam *param);
    ~KartObjectBike() override;
    KartBody *createBody(KartPhysics *physics) override;
    void createTires() override;
};

} // namespace Kart
