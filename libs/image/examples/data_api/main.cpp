#include <glm/glm.hpp>
#include <image/data/API.hpp>

using namespace image;

namespace image {

    struct Example {
        I32 a;
        F32 b;
        Path c;

        glm::vec2 vec;
    };

    TYPE(Example, {
        name = "Example";
        description = "Example type";
    });

    PROPERTY(Example, a, { a.description = "Test property 1"; });
    PROPERTY(Example, b, { b.description = "Test property 2"; });
    PROPERTY(Example, c, { c.description = "Test property 3"; });

    PROPERTY_FN(Example, F32, vec, { vec.name = "Vector"; }, ex, { return ex.vec.x; });

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
    ex.vec = { 0.5, 1.5 };
    std::cout << "ex.a (static) = " << ex.a << std::endl;
    DynamicRef ref { &ex };
    std::cout << "ex.a (dynamic) = " << *ref["a"].get<I32>() << std::endl;

    *ref["vec"].get<F32>() = 5.123;

    std::cout << "ToString: " << ifaceImpl<ToString>(ref)->toString(ref) << std::endl;

    return 0;
}
