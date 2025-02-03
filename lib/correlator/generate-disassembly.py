Import('env', 'projenv')

#print(projenv.Dump())

projenv.AddCustomTarget(
    name="libcorrelator-s",
    dependencies=None,
    actions=[
        "$CXX -g -o correlator.S $CXXFLAGS $CCFLAGS -S -fverbose-asm lib/correlator/src/correlator.cpp",
    ],
    title="Disassemble libcorrelator",
    description="Disassemble libcorrelator"
)