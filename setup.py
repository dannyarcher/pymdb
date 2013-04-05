from setuptools import find_packages, setup, Feature, Extension

pymdbbin = Feature(
    'pymdbbin module python biniding of mdbtools',
    standard=True,
    ext_modules = [
        Extension('pymdbbin', sources = [
            'pymdbbin.cpp', 
            'mdbtools/src/libmdb/catalog.c',
            'mdbtools/src/libmdb/dump.c',
            'mdbtools/src/libmdb/iconv.c',
            'mdbtools/src/libmdb/like.c',
            'mdbtools/src/libmdb/options.c',
            'mdbtools/src/libmdb/sargs.c',
            'mdbtools/src/libmdb/table.c',
            'mdbtools/src/libmdb/write.c',
            'mdbtools/src/libmdb/backend.c',
            'mdbtools/src/libmdb/data.c',
            'mdbtools/src/libmdb/file.c',
            'mdbtools/src/libmdb/index.c',
            'mdbtools/src/libmdb/map.c',
            'mdbtools/src/libmdb/money.c',
            'mdbtools/src/libmdb/props.c',
            'mdbtools/src/libmdb/stats.c',
            'mdbtools/src/libmdb/worktable.c'
            ],
            libraries=['glib-2.0'],
            include_dirs=[
                'mdbtools/include', 
                '../../mdbtools/include', 
                '/usr/local/include', 
                "/usr/local/include/glib-2.0",
                "/usr/local/Cellar/glib/2.34.3/lib/glib-2.0/include",
                "/usr/include/glib-2.0",
                "/usr/lib/glib-2.0/include",
                "/usr/lib/x86_64-linux-gnu/glib-2.0/include"]
        )
    ]
)

pymdb = Feature(
        'pymdb module python biniding of mdbtools',
        standard=True,
        py_modules=['pymdb']
)


setup (name = 'pymdb',
        version = '0.1.73',
        description = 'pymdb module python biniding of mdbtools',
        author="Danny Archer",
        author_email="danny@inbeta.org",
        license="LGPL",
        keywords = "microsoft, mdb, access, database, python",
        classifiers = [
            "Intended Audience :: Developers",
            "Operating System :: OS Independent",
            "Programming Language :: Python :: 2.7",
            "Programming Language :: Python :: 3.3",
            "Topic :: Software Development :: Libraries :: Python Modules",
            "Topic :: Utilities"
        ],

        packages = find_packages(),
        features={'pymdbbin': pymdbbin , 'pymdb':pymdb},
        install_requires=['distribute'],
        tests_require=['nose'],
        test_suite='nose.collector'
)

        