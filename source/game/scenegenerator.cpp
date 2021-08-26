#include "scenegenerator.h"
#include "../settings.h"

void SceneGenerator::randomiseObjects()
{
    m_objects.clear();
    for (u32 i = 0; i < gSettings.m_sceneObjCount.m_x; i++) {
        while (true) {
            u32 rng = util::getRandom<u32>(0, 6);
            if (rng < 2) {
                if (gSettings.m_spawnMode & ObjectSpawnMode::Cube) {
                    Object obj;
                    obj.m_type = ObjectType::Cube;
                    obj.randomise();
                    m_objects.push_back(obj);
                    break;
                }
            } else if (rng >= 2 && rng < 4) {
                if (gSettings.m_spawnMode & ObjectSpawnMode::Torus) {
                    Object obj;
                    obj.m_type = ObjectType::Torus;
                    obj.randomise();
                    m_objects.push_back(obj);
                    break;
                }
            } else if (rng >= 4) {
                if (gSettings.m_spawnMode & ObjectSpawnMode::Sphere) {
                    Object obj;
                    obj.m_type = ObjectType::Sphere;
                    obj.randomise();
                    m_objects.push_back(obj);
                    break;
                }
            }
        }
    }

    m_wfObjects.clear();
    for (u32 i = 0; i < gSettings.m_wfObjCount.m_x; i++) {
        while (true) {
            u32 rng = util::getRandom<u32>(0, 6);
            if (rng < 2) {
                if (gSettings.m_spawnMode & ObjectSpawnMode::Cube) {
                    Object obj;
                    obj.m_type = ObjectType::Cube;
                    obj.randomise();
                    m_wfObjects.push_back(obj);
                    break;
                }
            } else if (rng >= 2 && rng < 4) {
                if (gSettings.m_spawnMode & ObjectSpawnMode::Torus) {
                    Object obj;
                    obj.m_type = ObjectType::Torus;
                    obj.randomise();
                    m_wfObjects.push_back(obj);
                    break;
                }
            } else if (rng >= 4) {
                if (gSettings.m_spawnMode & ObjectSpawnMode::Sphere) {
                    Object obj;
                    obj.m_type = ObjectType::Sphere;
                    obj.randomise();
                    m_wfObjects.push_back(obj);
                    break;
                }
            }
        }
        m_wfObjects.back().m_wireFrame = true;
    }
}

void SceneGenerator::randomiseLights()
{
    m_lights.clear();
    for (u32 i = 0; i < gSettings.m_lightCount.m_x; i++) {
        Light light(i);
        light.randomise();
        m_lights.push_back(light);
    }
}