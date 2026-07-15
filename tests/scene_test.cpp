#include "scene.h"

#include <gtest/gtest.h>

TEST(SceneTest, CanBeCreatedWithoutOpeningAWindow) {
    Scene scene;

    SUCCEED();
}
