@echo off

if "%1"=="pull" (

    git pull origin install

    cd sdk
    git pull origin main

    cd bin
    git pull origin main

) else (

    echo ""
    echo "==== geode/sdk ===="
    echo ""

    cd sdk

    git add --all
    git commit -a
    git push origin main

    echo "==== geode/api ===="
    echo ""

    cd ..

    git add --all
    git commit -a
    git push origin install

)
