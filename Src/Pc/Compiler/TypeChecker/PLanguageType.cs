namespace Microsoft.Pc.TypeChecker
{
    public abstract class PLanguageType
    {
        public PLanguageType(TypeKind kind)
        {
            TypeKind = kind;
        }

        /// <summary>
        ///     The category of type this is (eg. sequence, map, base)
        /// </summary>
        public TypeKind TypeKind { get; set; }

        /// <summary>
        ///     Original representation of the type in P.
        /// </summary>
        public abstract string OriginalRepresentation { get; }

        /// <summary>
        ///     Representation of the type with typedefs and event sets expanded.
        /// </summary>
        public abstract string CanonicalRepresentation { get; }
    }
}