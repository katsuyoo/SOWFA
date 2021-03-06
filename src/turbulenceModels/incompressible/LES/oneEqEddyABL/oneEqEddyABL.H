/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::incompressible::LESModels::oneEqEddyABL

Description
    One Equation Eddy Viscosity Model for incompressible atmospheric flows. 
    It is based on the original oneEqEddy OpenFOAM model, but has some small
    modifications, such as buoyancy production, specific to atmospheric flow.

    Eddy viscosity SGS model using a modeled balance equation to simulate the
    behaviour of k, hence,
    \verbatim
        d/dt(k) 
      + div(U*k)
        =
      - dev(R)*(grad(U)
      + (1/theta_0)*g&q
      + div(2*nuSgs*grad(k))
      - eps

    and

        R = 2/3*k*I - 2*nuSgs*dev(D)
        Reff = 2/3*k*I - 2*nuEff*dev(D)
        eps = ce*k^(3/2)/l

    where

        D = symm(grad(U));
        nuSgs = ck*sqrt(k)*delta
        nuEff = nuSgs + nu
        theta_0 = reference potential temperature
        g = gravity
        q = -2*(nuSgs/Pr_t)*grad(T)
        l = length scale
    \endverbatim

SourceFiles
    oneEqEddyABL.C

\*---------------------------------------------------------------------------*/

#ifndef oneEqEddyABL_H
#define oneEqEddyABL_H

#include "GenEddyViscABL.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace incompressible
{
namespace LESModels
{

/*---------------------------------------------------------------------------*\
                           Class oneEqEddyABL Declaration
\*---------------------------------------------------------------------------*/

class oneEqEddyABL
:
    public GenEddyViscABL
{
    // Private data

        volScalarField k_;

        dimensionedScalar ck_;


    // Model coefficient field (necessary if the coefficient becomes variable).

        volScalarField ceField_;


    // Private Member Functions

        //- Update sub-grid scale fields
        void updateSubGridScaleFields();

        // Disallow default bitwise copy construct and assignment
        oneEqEddyABL(const oneEqEddyABL&);
        oneEqEddyABL& operator=(const oneEqEddyABL&);


public:

    //- Runtime type information
    TypeName("oneEqEddyABL");

    // Constructors

        //- Construct from components
        oneEqEddyABL
        (
            const volVectorField& U,
            const surfaceScalarField& phi,
            transportModel& transport,
            const word& turbulenceModelName = turbulenceModel::typeName,
            const word& modelName = typeName
        );


    //- Destructor
    virtual ~oneEqEddyABL()
    {}


    // Member Functions

        //- Return SGS kinetic energy
        virtual tmp<volScalarField> k() const
        {
            return k_;
        }

        //- Return sub-grid disipation rate
        virtual tmp<volScalarField> epsilon() const
        {
            return ceField_*k()*sqrt(k())/l_;
        }

        //- Return the effective diffusivity for k
        tmp<volScalarField> DkEff() const
        {
            return tmp<volScalarField>
            (
                new volScalarField("DkEff", nuSgs_ + nu())
            );
        }

        //- Correct Eddy-Viscosity and related properties
        virtual void correct(const tmp<volTensorField>& gradU);

        //- Read LESProperties dictionary
        virtual bool read();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace LESModels
} // End namespace incompressible
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
