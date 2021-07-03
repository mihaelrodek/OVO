using System;

namespace raytracing
{
    /// <summary>
    /// Klasa predstvlja scenu kod modela crtanja slike pomocu ray tracinga. Sastoji
    /// se od izvora svjetlosti i konacnog broja objekata.
    /// </summary>
    public class Scene
    {
        const int MAXDEPTH = 5; //maksimalna dubina rekurzije
        private int numberOfObjects;
        private Sphere[] sphere;
        private Point lightPosition;
        private ColorVector backgroundColors = new ColorVector(0, 0, 0);
        private ColorVector light = new ColorVector((float)1, (float)1, (float)1);
        private ColorVector ambientLight = new ColorVector((float)0.5, (float)0.5, (float)0.5);

        /// <summary>
        /// Inicijalni konstruktor koji postavlja poziciju svijetla i parametre svih
        /// objekata u sceni.
        /// </summary>
        /// <param name="lightPosition">pozicija svijetla</param>
        /// <param name="numberOfObjects">broj objekata u sceni</param>
        /// <param name="sphereParameters">parametri svih kugli</param>
        public Scene ( Point lightPosition, int numberOfObjects, SphereParameters[] sphereParameters )
        {
            this.lightPosition = lightPosition;
            this.numberOfObjects = numberOfObjects;
            sphere = new Sphere[numberOfObjects];
            for(int i = 0; i < numberOfObjects; i++)
            {
                sphere[i] = new Sphere(sphereParameters[i]);
            }
        }

        /// <summary>
        /// Metoda provjerava da li postoji sjena na tocki presjeka. Vraca true ako
        /// se zraka od mjesta presjeka prema izvoru svjetlosti sjece s nekim objektom.
        /// </summary>
        /// <param name="intersection">tocka presjeka</param>
        /// <returns>true ako postoji sjena u tocki presjeka, false ako ne postoji</returns>
        private bool shadow ( Point intersection )
        {
            Ray shadowRay = new Ray(intersection, lightPosition);

            for (int i = 0; i < this.numberOfObjects; i++)
            {
                if (sphere[i].intersection(shadowRay))
                {
                    return true;
                }
            }

            return false;
        }

		/// <summary>
		/// Metoda koja pomocu pracenja zrake racuna boju u tocki presjeka. Racuna se
		/// osvjetljenje u tocki presjeka te se zbraja s doprinosima osvjetljenja koje
		/// donosi reflektirana i refraktirana zraka.
		/// </summary>
		/// <param name="ray">pracena zraka</param>
		/// <param name="depth">dubina rekurzije</param>
		/// <returns>vektor boje u tocki presjeka</returns>
		public ColorVector traceRay(Ray ray, int depth)
		{
			ColorVector C = new ColorVector(0, 0, 0);
			int closest = -1;
			bool found = false;

			//ako je dubina>max. dubine, prekini funkciju i vrati crnu boju
			if (depth > MAXDEPTH)
			{
				return new ColorVector(0, 0, 0);
			}

			//nađi najbliži presjek zrake R sa scenom
			for (int i = 0; i < this.numberOfObjects; i++)
			{
				if (sphere[i].intersection(ray))
				{
					found = true;

					if (closest == -1 || ray.getStartingPoint().getDistanceFrom(sphere[i].getIntersectionPoint()) < ray.getStartingPoint().getDistanceFrom(sphere[closest].getIntersectionPoint()))
					{
						closest = i;
					}
				}
			}

			//ako nema presjeka, prekini funkciju i vrati kao rezultat boju pozadine
			if (!found)
			{
				return backgroundColors;
			}

			//izračunaj boju lokalnog osvjetljenja Clocal u točki presjeka
			ColorVector Clocal = new ColorVector(0, 0, 0);

			Point intersectionPoint = sphere[closest].getIntersectionPoint();

			Vector L = new Vector(intersectionPoint, lightPosition);
			Vector N = sphere[closest].getNormal(intersectionPoint);
			Vector V = new Vector(intersectionPoint, ray.getStartingPoint());
			
			L.normalize();
			N.normalize();
			V.normalize();
			
			double nI = sphere[closest].getNi();
			if (V.dotProduct(N) < 0)
			{
				N = N.multiple(-1);
				nI = 1 / nI;
			}

			Vector R = L.getReflectedVector(N);

			R.normalize();

			//ambijentna komponenta
			Clocal = Clocal.add(ambientLight.multiple(sphere[closest].getKa()));

			if (L.dotProduct(N) >= 0 && !shadow(intersectionPoint))
			{
                //difuzna komponenta
                Clocal = Clocal.add(light.multiple(sphere[closest].getKd()).multiple(L.dotProduct(N)));
                //spekularna komponenta
                Clocal = Clocal.add(light.multiple(sphere[closest].getKs()).multiple(Math.Pow(R.dotProduct(V), sphere[closest].getN())));
            }

			//izračunaj odbijenu zraku Rrefl
			Ray Rrefl = new Ray(intersectionPoint, V.getReflectedVector(N));

			//boja Crefl = TraceRay(Rrefl, dubina+1)
			ColorVector Crefl = traceRay(Rrefl, depth + 1);

			//izračunaj refraktiranu zraku Rrefr
			Ray Rrefr = new Ray(intersectionPoint, V.getRefractedVector(N, nI));

			//boja Crefr = TraceRay(Rrefr, dubina+1)
			ColorVector Crefr = traceRay(Rrefr, depth + 1);

            //boja C = kombinacija(Clocal, Crefl, Crefr)
            C = C.add(Clocal);
            C = C.add(Crefl.multiple(sphere[closest].getReflectionFactor()));
            C = C.add(Crefr.multiple(sphere[closest].getRefractionFactor()));

            C.correct();

			return C;
		}
	}
}