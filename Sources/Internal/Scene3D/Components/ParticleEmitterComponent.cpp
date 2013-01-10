#include "Scene3D/Components/ParticleEmitterComponent.h"
#include "Particles/ParticleEmitter.h"

namespace DAVA
{

ParticleEmitterComponent::ParticleEmitterComponent()
:	particleEmitter(0)
{

}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	SafeRelease(particleEmitter);
}

Component * ParticleEmitterComponent::Clone()
{
	ParticleEmitterComponent * newComponent = new ParticleEmitterComponent();
	newComponent->SetParticleEmitter(particleEmitter->Clone());
	return newComponent;
}

void ParticleEmitterComponent::SetParticleEmitter(ParticleEmitter * _particleEmitter)
{
	SafeRelease(particleEmitter);
	particleEmitter = SafeRetain(_particleEmitter);
}

ParticleEmitter * ParticleEmitterComponent::GetParticleEmitter()
{
	return particleEmitter;
}

}