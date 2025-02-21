#include "object.hpp"
#include "renderer.hpp"

VulkanObject::VulkanObject(VulkanRenderer* renderer) : device(renderer->getDevice()) {

}