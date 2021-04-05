#include <image/data/API.hpp>

using namespace image;

namespace image {

    struct Example {
        I32 a;
        F32 b;
        Path c;
    };

    TYPE(Example, {
        name = "Example";
        description = "Example type";
    });

    PROPERTY(Example, a, { a.description = "Test property 1"; });
    PROPERTY(Example, b, { b.description = "Test property 2"; });
    PROPERTY(Example, c, { c.description = "Test property 3"; });

    INTERFACE(ToString, Example, ToStringImpl<Example>);

}

int main(int, const char *[]) {
    for (auto &&[name, info] : TypeRegistry::types) {
        std::cout << name << std::endl;
    }

    std::cout << "Example is (static): " << dynInfo<Example>().description << std::endl;
    std::cout << "Example is (dynamic): " << dynInfo("Example").description << std::endl;
    std::cout << "Example.a is (static): " << dynInfo<Example>()["a"].description << std::endl;
    std::cout << "Example.a is (dynamic): " << dynInfo("Example")["a"].description << std::endl;

    Example ex;
    ex.a = 123;
    ex.b = 3.1415;
    ex.c = "./test/path";
    std::cout << "ex.a (static) = " << ex.a << std::endl;
    DynamicRef ref { &ex };
    std::cout << "ex.a (dynamic) = " << *ref["a"].get<I32>() << std::endl;

    std::cout << "ToString: " << ifaceImpl<ToString>(ref)->toString(ref) << std::endl;

    return 0;
}
