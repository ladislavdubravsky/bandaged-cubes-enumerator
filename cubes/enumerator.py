def enumerate_analytic():
    """
    A face plane is a plane separating a cube face (9 cubies) from
    the rest of the cube (18 cubies). Now:
    1. Start with set S containing one element, the fully bandaged 3x3x3 block
       (i.e., puzzle isomorphic to 1x1x1 cube)
    2. Pick any element from S and split in two it by cutting it by a face
       plane. Replace the original block in S by the two new blocks.
    3. Either end the process or return to step 2.
    We'll call any partition of 3x3x3 cube into blocks resulting from the
    process above a bandage shape.
    With a few other tweaks to be spotted below, how many distinct bandage
    shapes are there?
    """
    p1 = 1
    p2 = 1 + p1**2
    p3 = 1 + 2*p2*p1 - 1
    p22 = 1 + 2*p2**2 - 1
    p32 = 1 + 2*p22*p2 + p3**2 - p2**3 - 2*p2**2 + 1
    p33 = p32*p3
    p222 = 1 + 3*p22**2 - 3*p2**4 + 1
    p322 = 1 + 2*p32**2 + 2*p222*p22 - p3**4 - 4*p2**2*p22**2 - p22**3 + 2*p2**4 + 2*p2**6 - 1
    p332 = p33**2 + p322*p32 - p32**2*p3**2
    p333 = 1 + p332*p33
    return p333
