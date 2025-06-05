 #include "FramebufferLibrary.hpp"
 #include "FramebufferGenerator.hpp"

 void FramebufferLibrary::AddFramebufferGenerator(std::string name, FramebufferGenerator* framebufferGenerator) {
    framebufferGenerators[name] = framebufferGenerator;          
}

FramebufferGenerator* FramebufferLibrary::GetFramebufferGenerator(const std::string& name) {
    return framebufferGenerators.at(name);
}

void FramebufferLibrary::Cleanup(){
    for(auto& [key,val]: framebufferGenerators){
        val->Cleanup();
        delete(val); //framebuffer generators allocated with new
    }
}