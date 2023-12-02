def getBranch() {
    if (env.BRANCH_NAME != null && !env.BRANCH_NAME.isEmpty() ) {
        return env.BRANCH_NAME
    } else {
        return 'develop'
    }
}

void checkout_tgc_vp() {
    checkout([
        $class: 'GitSCM',
        branches: [
            [name: 'refs/heads/' + getBranch()]
        ],
        extensions: [
            [$class: 'CleanBeforeCheckout'],
            [$class: 'SubmoduleOption', 
                disableSubmodules: false, 
                recursiveSubmodules: true, 
                trackingSubmodules: false,
                parentCredentials: true, 
                shallow: true
            ]
        ],
        submoduleCfg: [],
        userRemoteConfigs: [
            [url: 'https://github.com/Minres/TGC-VP.git']
        ]
    ])
}

void build_tgc_vp() {
    try {
        sh("conan profile new default --detect --force")
        sh("conan profile update settings.compiler.libcxx=libstdc++11 default")
        sh("conan remote add minres https://git.minres.com/api/packages/Tooling/conan --force")
        sh("cmake --version")
    }
    catch (exc) {
        echo 'Conan configured'
    }
    sh("rm -rf build")
    sh("git submodule update --recursive")
    sh("cmake -S . -B build  -DWITH_TCC=OFF && cmake --build build -j16")
    fingerprint 'build/src/tgc-vp'
}

pipeline {
    agent none

    options { 
        // using the Timestamper plugin we can add timestamps to the console log
        timestamps() 
        skipStagesAfterUnstable() 
    }

    stages {
        stage('tgc-VP pipeline') {
           parallel {
            stage('ubuntu20'){
                agent {docker { image 'ubuntu-20.04' } }
                stages {
                    stage('Checkout on Ubuntu20.04') { steps {    checkout_tgc_vp() }}
                    stage('Build') { steps {    build_tgc_vp() }    }
                }
            }
            stage('ubuntu22'){
                agent {docker { image 'ubuntu-22.04' } }
                stages {
                    stage('Checkout on Ubuntu22.04') { steps {    checkout_tgc_vp() }}
                    stage('Build') { steps {    build_tgc_vp() }    }
                }
            }
            stage('Fedora28'){
                agent {docker { image 'fedora28' } }
                stages {
                    stage('Checkout on Fedora') { steps {checkout_tgc_vp()}}
                    stage('Build') { steps {build_tgc_vp()    }}
                }
            }
            stage('CentOS7'){
                agent {docker { image 'centos7' } }
                stages {
                    stage('Checkout on Ubuntu') { steps {checkout_tgc_vp()}}
                    stage('Build') { steps {build_tgc_vp()}
                    }
                }
            }
         }
        }
    }
}