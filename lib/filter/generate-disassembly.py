Import('env', 'projenv')

#print(projenv.Dump())

projenv.AddCustomTarget(
    name="libfilter-s",
    dependencies=None,
    actions=[
        "$CXX -g -o filter.S $CXXFLAGS $CCFLAGS -S -fverbose-asm lib/filter/src/filter.cpp",
        "$CXX -g -o cic.S $CXXFLAGS $CCFLAGS -S -fverbose-asm lib/filter/src/cic.cpp"
    ],
    title="Disassemble libfilter",
    description="Disassemble libfilter"
)