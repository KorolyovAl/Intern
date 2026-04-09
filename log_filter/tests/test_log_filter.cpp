#include <catch2/catch_all.hpp>

#include <sstream>

#include "../src/log_filter.h"

namespace {

std::string RunFilter(log_filter::Filter& filter, std::string_view input) {
    std::stringstream is{std::string(input)};
    std::ostringstream os;
    filter.StartProcessing(is, os);
    return os.str();
}

} // namespace

SCENARIO("Check creating filter with incorrect initial data: must throw exceptions") {
    using namespace log_filter;

    WHEN("range format is invalid") {
        REQUIRE_THROWS(create_filter({
            {"type", "range", "value", "10.0.0.1/10.0.0.255"}
        }));

        REQUIRE_THROWS(create_filter({
            {"type", "range", "value", "10.0.0.1/24-10.0.0.255"}
        }));
    }

    WHEN("left range boundary is greater than right") {
        REQUIRE_THROWS(create_filter({
            {"type", "range", "value", "10.0.0.255-10.0.0.1"}
        }));
    }

    WHEN("prefix is greater than 32") {
        REQUIRE_THROWS(create_filter({
            {"type", "subnet", "value", "10.0.0.0/33"}
        }));
    }

    WHEN("IP format is invalid") {
        REQUIRE_THROWS(create_filter({
            {"type", "subnet", "value", "300.0.0.0/24"}
        }));

        REQUIRE_THROWS(create_filter({
            {"type", "subnet", "value", "100.0/24"}
        }));
    }

    WHEN("filter type is unknown") {
        REQUIRE_THROWS(create_filter({
            {"type", "unknown", "value", "10.0.0.0/24"}
        }));
    }
}

SCENARIO("Check filter type: range") {
    using namespace log_filter;

    GIVEN("a filter with one range rule") {
        auto filter = create_filter({        
            {"type", "range", "value", "10.0.0.1-10.0.0.255"}
        });        

        WHEN("IP is inside the range") {
            REQUIRE(RunFilter(filter, "10.0.0.42") == "10.0.0.42\n");
        }

        WHEN("IP is outside the range") {
            REQUIRE(RunFilter(filter, "10.0.1.1").empty());
        }
    }

    GIVEN("a filter with two range rules combained with AND") {
        auto filter = create_filter({        
            {"type", "range", "value", "10.0.0.0-100.0.0.0"},
            {"type", "range", "value", "10.0.0.1-10.0.0.255"}
        });        

        WHEN("IP belongs to both ranges") {
            REQUIRE(RunFilter(filter, "10.0.0.100") == "10.0.0.100\n");
        }

        WHEN("IP belongs only to the wider range") {
            REQUIRE(RunFilter(filter, "50.0.0.1").empty());
        }

        WHEN("IP belongs to neither range") {
            REQUIRE(RunFilter(filter, "200.0.0.1").empty());
        }
    }
}

SCENARIO("Check filter type: subnet") {
    using namespace log_filter;

    GIVEN("a filter with one subnet rule") {
        auto filter = create_filter({
            {"type", "subnet", "value", "10.0.0.0/24"}
        });

        WHEN("IP is inside the subnet") {
            REQUIRE(RunFilter(filter, "10.0.0.42") == "10.0.0.42\n");
        }

        WHEN("IP is the subnet boundary adress") {
            REQUIRE(RunFilter(filter, "10.0.0.0") == "10.0.0.0\n");
        }

        WHEN("IP is outside the subnet") {
            REQUIRE(RunFilter(filter, "10.0.1.42").empty());
        }
    }

    GIVEN("a filter with two subnet rules combined with AND") {
        auto filter = create_filter({
            {"type", "subnet", "value", "10.0.0.0/16"},
            {"type", "subnet", "value", "10.0.0.0/24"}
        });

        WHEN("IP belongs to both subnets") {
            REQUIRE(RunFilter(filter, "10.0.0.42") == "10.0.0.42\n");
        }

        WHEN("IP belongs only to the one subnet") {
            REQUIRE(RunFilter(filter, "10.0.1.42").empty());
        }

        WHEN("IP belongs to neither subnet") {
            REQUIRE(RunFilter(filter, "11.0.1.42").empty());
        }
    }
}

SCENARIO("Check filter with mixed conditions: range and subnet") {
    using namespace log_filter;

    GIVEN("a filter with range and subnet rules combined with AND") {
        auto filter = create_filter({
            {"type", "range", "value", "10.0.0.1-10.0.1.255"},
            {"type", "subnet", "value", "10.0.0.0/24"}
        });

        WHEN("IP belongs to both conditions") {
            REQUIRE(RunFilter(filter, "10.0.0.42") == "10.0.0.42\n");
        }

        WHEN("IP belongs only to the range") {
            REQUIRE(RunFilter(filter, "10.0.1.42").empty());
        }

        WHEN("IP belongs to neither condition") {
            REQUIRE(RunFilter(filter, "11.0.0.42").empty());
        }
    }
}

SCENARIO("Check filter with boundary conditions") {
    using namespace log_filter;

    GIVEN("a range rule with exact lower and upper bounds") {
        auto filter = create_filter({
            {"type", "range", "value", "10.0.0.1-10.0.0.255"}
        });

        WHEN("IP is equal to the lower boundary") {
            REQUIRE(RunFilter(filter, "10.0.0.1") == "10.0.0.1\n");
        }

        WHEN("IP is equal to the upper boundary") {
            REQUIRE(RunFilter(filter, "10.0.0.255") == "10.0.0.255\n");
        }

        WHEN("IP is just below the lower boundary") {
            REQUIRE(RunFilter(filter, "10.0.0.0").empty());
        }

        WHEN("IP is just above the upper boundary") {
            REQUIRE(RunFilter(filter, "10.0.1.0").empty());
        }
    }

    GIVEN("a subnet rule with /32 mask") {
        auto filter = create_filter({
            {"type", "subnet", "value", "10.0.0.42/32"}
        });

        WHEN("IP matches exactly") {
            REQUIRE(RunFilter(filter, "10.0.0.42") == "10.0.0.42\n");
        }

        WHEN("IP differs by one") {
            REQUIRE(RunFilter(filter, "10.0.0.43").empty());
        }
    }

    GIVEN("a subnet rule with /0 mask") {
        auto filter = create_filter({
            {"type", "subnet", "value", "0.0.0.0/0"}
        });

        WHEN("any IPv4 address is passed") {
            REQUIRE(RunFilter(filter, "192.168.1.10") == "192.168.1.10\n");
        }
    }

    GIVEN("a subnet rule with the lowest IPv4 boundary") {
        auto filter = create_filter({
            {"type", "subnet", "value", "0.0.0.0/0"}
        });

        WHEN("IP is 0.0.0.0") {
            REQUIRE(RunFilter(filter, "0.0.0.0") == "0.0.0.0\n");
        }

        WHEN("IP is 255.255.255.255") {
            REQUIRE(RunFilter(filter, "255.255.255.255") == "255.255.255.255\n");
        }
    }
}

SCENARIO("Check filter with incorrect logs") {
    using namespace log_filter;

    GIVEN("a filter without conditions") {
        auto filter = create_filter({});

        WHEN("IP is incorrect") {
            REQUIRE(RunFilter(filter, "1232143").empty());
            REQUIRE(RunFilter(filter, "asdew").empty());
            REQUIRE(RunFilter(filter, "111.111.111.a").empty());
        }
    }
}

SCENARIO("Check filter without conditions") {
    using namespace log_filter;

    GIVEN("a filter without conditions") {
        auto filter = create_filter({});

        WHEN("IP is valid") {
            REQUIRE(RunFilter(filter, "10.0.0.42") == "10.0.0.42\n");
        }
    }
}

SCENARIO("Check filter with comments") {
    using namespace log_filter;

    auto filter = create_filter({
        {"type", "subnet", "value", "10.0.0.0/24"}
    });

    WHEN("simple comment comment") {
        REQUIRE(RunFilter(filter, "10.0.0.42 - comment") == "10.0.0.42 - comment\n");
    }

    WHEN("comment contains a single hyphen inside") {
        REQUIRE(RunFilter(filter, "10.0.0.42 - foo-bar") == "10.0.0.42 - foo-bar\n");
    }
}