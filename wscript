# -*- mode: python -*-

def options(opt):
    opt.load('compiler_cxx')
    #opt.load('unittest_gtest doxygen', tooldir='wafextra')

    opt.add_option('--boost-incpath', action='store')

    opt.add_option('--sndfile-incpath', action='store')
    opt.add_option('--sndfile-libpath', action='store')

    opt.add_option('--yamlcpp-incpath', action='store')
    opt.add_option('--yamlcpp-libpath', action='store')

def configure(conf):
    for envname in 'debug', 'release':
        conf.setenv(envname)
        conf.load('compiler_cxx')
        #conf.load('unittest_gtest doxygen', tooldir='wafextra')
        conf.check_cxx(lib='sndfile',
                       includes=[conf.options.sndfile_incpath],
                       libpath=[conf.options.sndfile_libpath],
                       uselib_store='SNDFILE')
        conf.check_cxx(lib='yaml-cpp',
                       includes=[conf.options.yamlcpp_incpath],
                       libpath=[conf.options.yamlcpp_libpath],
                       uselib_store='YAMLCPP')
        conf.check_cc(fragment='''
#include <boost/version.hpp>
    int main() { return 0; }
        ''',
                      includes=[conf.options.boost_incpath],
                      msg="Checking boost headers",
                      uselib_store='BOOST_HEADERS')

        if envname == 'debug':
            conf.env.CFLAGS = ['-g']
            conf.env.CXXFLAGS = ['-g', '-std=c++11']
        elif envname == 'release':
            conf.env.CFLAGS = ['-O3']
            conf.env.CXXFLAGS = ['-O3', '-std=c++11']

def build(bld):
    if not bld.variant: 
        bld.fatal('specify variant')

    bld.program(source='src/tool/feed.cpp', 
                target='gear_feed',
                includes='include/ 3rd/',
                cxxflags=['-Wno-c++11-extensions'],
                use='gear SNDFILE YAMLCPP BOOST_HEADERS')

    bld.shlib(source='''
src/lib/io/logging.cpp 
src/lib/io/fileformat.cpp 
src/lib/io/matrix.cpp 
src/lib/node/fft.cpp
src/lib/node/noise.cpp
src/lib/node/affinetrans.cpp
src/lib/node/window.cpp
src/lib/node/concat.cpp
src/lib/node/energy.cpp
src/lib/node/htkcompat.cpp
src/lib/node/melfb.cpp
src/lib/node/elemfunc.cpp
src/lib/node/delta.cpp
src/lib/node/node.cpp
src/lib/node/meannorm.cpp
src/lib/node/factory.cpp
src/lib/flow/flow.cpp
src/lib/flow/preset.cpp
''',
              includes='include/ 3rd/',
              cxxflags=['-Wno-c++11-extensions'],
              target='gear',
              use='SNDFILE YAMLCPP BOOST_HEADERS',
              install_path="${PREFIX}/lib")

    bld.install_files('${PREFIX}',
                      bld.path.ant_glob('include/gear/**/*.hpp'),
                      cwd=bld.path, relative_trick=True)
      
    tests = [
        ('node', 'window'),
        ('node', 'fft'),
        ('node', 'melfb'),
        ('node', 'elemfunc'),
        ('node', 'fir'),
        ('node', 'concat'),
        ('node', 'energy'),
        ('node', 'meannorm'),
        ('node', 'thru'),
        ('node', 'factory'),
        ('io', 'fileformat')
    ]
    '''
    for subdir, test in tests:
        bld.program(features = 'gtest',
                    source = 'src/test/{}/test_{}.cpp'.format(subdir, test),
                    includes='include/ 3rd/',
                    target = 'gear_test_{}_{}'.format(subdir.replace('/', '_'),
                                                      test),
                    defines = 'ENABLE_TRACE',
                    cxxflags=['-Wno-c++11-extensions'],
                use = 'gear SNDFILE YAMLCPP BOOST_HEADERS')
    '''
    
    if bld.env.DOXYGEN:
        bld(features="doxygen", doxyfile="Doxyfile")

from waflib.Build import BuildContext, CleanContext, \
        InstallContext, UninstallContext

for x in 'debug release'.split():
    for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
        name = y.__name__.replace('Context','').lower()
        class tmp(y):
            cmd = name + '_' + x
            variant = x
