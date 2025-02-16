#pragma once

#include "physics.hpp"
#include "input.hpp"
#include "window.hpp"
#include <thread>

namespace UnitTests
{
    void angleInverse()
    {
        using Eigen::Rotation2Dd;

        Rotation2Dd halfpi{PI/2};
        assert(halfpi.inverse().smallestAngle() == -PI/2);

        Rotation2Dd threefourthspi{3*PI/4};
        assert(threefourthspi.inverse().smallestAngle() == -3*PI/4);

        Rotation2Dd oneandahalfpi{3*PI/2};
        assert(oneandahalfpi.inverse().smallestAngle() == PI/2);
    }

    void vectorRotate()
    {
        using Eigen::Vector2d;
        using Eigen::Rotation2Dd;

        Rotation2Dd halfpi{PI/2};
        Vector2d yOne{0, 1};

        Vector2d xMinusOne{-1, 0};

        assert((halfpi * yOne).isApprox(xMinusOne));

        Rotation2Dd minusHalfpi{-PI/2};

        assert((minusHalfpi * xMinusOne).isApprox(yOne));

        Rotation2Dd quarterPi{PI/4};

        Vector2d xOne{1, 0};

        Vector2d unitVectAtQuarterPi{1, 1};
        unitVectAtQuarterPi.normalize();
        
        assert((quarterPi * xOne).isApprox(unitVectAtQuarterPi));

        Vector2d yTwo{0, 2};

        Vector2d xMinusTwo{-2, 0};

        assert((halfpi * yTwo).isApprox(xMinusTwo));

        Vector2d unitVectAtThreeQuartersPi{-1, -1};
        unitVectAtThreeQuartersPi.normalize();

        Rotation2Dd threeQuartersPi{5*PI/4};

        /*Debug::log(std::to_string((threeQuartersPi * xOne).x()) + "; "
            + std::to_string((threeQuartersPi * xOne).y()));*/

        assert((threeQuartersPi * xOne).isApprox(unitVectAtThreeQuartersPi));

        Vector2d unitVectAtMinusQuarterPi{1, -1};
        unitVectAtMinusQuarterPi.normalize();

        assert((halfpi * unitVectAtThreeQuartersPi).isApprox(unitVectAtMinusQuarterPi));
    }
}