///////////////////////////////////////////////////////////////////
// DiscLayer.cxx, ACTS project
///////////////////////////////////////////////////////////////////

// Geometry module
#include "ACTS/Layers/DiscLayer.h"
#include "ACTS/Detector/GenericApproachDescriptor.h"
#include "ACTS/Detector/GenericOverlapDescriptor.h"
#include "ACTS/Material/SurfaceMaterial.h"
#include "ACTS/Volumes/AbstractVolume.h"
#include "ACTS/Volumes/CylinderVolumeBounds.h"
#include "ACTS/Volumes/BoundarySurfaceFace.h"
#include "ACTS/Surfaces/DiscBounds.h"
#include "ACTS/Surfaces/RadialBounds.h"
#include "ACTS/Utilities/BinUtility.h"
// Core module
#include "ACTS/Utilities/AlgebraDefinitions.h"

Acts::DiscLayer::DiscLayer(std::shared_ptr<Acts::Transform3D> transform,
                          std::shared_ptr<const Acts::DiscBounds> dbounds,
                          Acts::SurfaceArray* surfaceArray,
                          double thickness,
                          Acts::OverlapDescriptor* olap,
                          Acts::ApproachDescriptor* ades,
                          int laytyp) :
  DiscSurface(transform, dbounds),
  Layer(surfaceArray, thickness, olap, ades, laytyp)
{
    
    // just create a generic overlap descriptor if none is there
    if (!Layer::m_overlapDescriptor) Layer::m_overlapDescriptor = new GenericOverlapDescriptor();
    // create the representing volume
    const Acts::RadialBounds* rBounds = dynamic_cast<const Acts::RadialBounds*>(dbounds.get());
    if (rBounds){
       // @TODO make a trapezoidal volume when you have DiscTrapezoidalBounds 
       CylinderVolumeBounds* cvBounds = new CylinderVolumeBounds(rBounds->rMin(),
                                                                 rBounds->rMax(),
                                                                 0.5*thickness);
       Layer::m_representingVolume = new AbstractVolume(transform,VolumeBoundsPtr(cvBounds));
    }
    // associate teh layer to this
    DiscSurface::associateLayer(*this);
    if (!ades && surfaceArray) buildApproachDescriptor();
    // register the layer
    if (ades) m_approachDescriptor->registerLayer(*this);
    
}
                
Acts::DiscLayer::DiscLayer(const Acts::DiscLayer& dlay, const Acts::Transform3D& transf):
  DiscSurface(dlay,transf),
  Layer(dlay)
{
    DiscSurface::associateLayer(*this);
    if (m_surfaceArray) buildApproachDescriptor();    
}
    
const Acts::DiscSurface& Acts::DiscLayer::surfaceRepresentation() const
{
  return (*this);
}

/** build approach surfaces */
void Acts::DiscLayer::buildApproachDescriptor() const {
    // delete it
    delete m_approachDescriptor;
    // delete the surfaces    
    // take the boundary surfaces of the representving volume if they exist
    if (m_representingVolume){
        // get teh boundary surfaces
        const std::vector< std::shared_ptr<const Acts::BoundarySurface<Acts::AbstractVolume> > >& bSurfaces = m_representingVolume->boundarySurfaces();
        // fill in the surfaces into the vector
        std::vector< std::shared_ptr<const Acts::BoundarySurface<Acts::AbstractVolume> > >* aSurfaces = new std::vector< std::shared_ptr<const Acts::BoundarySurface<Acts::AbstractVolume> > >;
        aSurfaces->push_back(bSurfaces[negativeFaceXY]);
        aSurfaces->push_back(bSurfaces[positiveFaceXY]);
        // create an ApproachDescriptor with Boundary surfaces
        m_approachDescriptor = new Acts::GenericApproachDescriptor<const BoundarySurface<AbstractVolume> >(aSurfaces);   
    } else {        
        // create the new surfaces - positions first
        Acts::Vector3D aspPosition(center()+0.5*thickness()*normal());
        Acts::Vector3D asnPosition(center()-0.5*thickness()*normal());
        Acts::Transform3D* asnTransform = new Acts::Transform3D(Acts::Translation3D(asnPosition));   
        Acts::Transform3D* aspTransform = new Acts::Transform3D(Acts::Translation3D(aspPosition));   
        // create the vector    
        std::vector< const Acts::Surface* > aSurfaces; 
        aSurfaces.push_back(new Acts::DiscSurface(std::shared_ptr<Acts::Transform3D>(asnTransform), m_bounds));
        aSurfaces.push_back(new Acts::DiscSurface(std::shared_ptr<Acts::Transform3D>(aspTransform), m_bounds));
        // create an ApproachDescriptor with standard surfaces surfaces - these will be deleted by the approach descriptor
        m_approachDescriptor = new Acts::GenericApproachDescriptor<const Acts::Surface>(aSurfaces);   
    }
    for (auto& sIter : (m_approachDescriptor->containedSurfaces())){
        sIter->associateLayer(*this);
    }
}