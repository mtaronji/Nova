 #include "FramebufferLibrary.hpp"
 #include "FramebufferGenerator.hpp"

 void FramebufferLibrary::AddFramebufferGenerator(std::string name, FramebufferGenerator&& framebufferGenerator) {
 
     framebufferGenerators.emplace(
         std::move(name),
         std::move(framebufferGenerator)
     );
}

FramebufferGenerator& FramebufferLibrary::GetFramebufferGenerator(const std::string& name) {
    return framebufferGenerators.at(name);
}

void FramebufferLibrary::Cleanup() {
    int i = 0;
    for(auto& [name, framebufferGenerator] : framebufferGenerators) {
        if(i != framebufferGenerator.GetSwapchainAttachmentIndex()) {
            framebufferGenerator.Cleanup();
		}
	}
}