module Styles = {
  open Css;

  let topicBar =
    style([
      width(`percent(100.)),
      display(`flex),
      flexDirection(`row),
      justifyContent(`spaceBetween),
    ]);
  let seeAll =
    style([
      display(`flex),
      flexDirection(`row),
      cursor(`pointer),
      Media.mobile([
        flexDirection(`column),
        justifyContent(`spaceBetween),
        alignItems(`flexEnd),
        selector("> *", [display(`block)]),
      ]),
    ]);
  let cFlex =
    style([display(`flex), flexDirection(`column), Media.mobile([marginTop(`px(10))])]);
  let rightArrow =
    style([
      width(`px(25)),
      marginTop(`px(17)),
      marginLeft(`px(16)),
      Media.mobile([margin(`zero)]),
    ]);

  let hScale = 20;
  let fullWidth = style([width(`percent(100.0)), display(`flex)]);
  let blockContainer = style([minWidth(`px(60))]);
  let statusContainer =
    style([
      minWidth(`percent(100.)),
      display(`flex),
      flexDirection(`row),
      alignItems(`center),
      justifyContent(`center),
    ]);

  let logo = style([width(`px(20))]);
};

let renderTitle = allSub => {
  <div className=Styles.topicBar>
    <Text
      value="Latest Transactions"
      size=Text.Xxl
      weight=Text.Bold
      block=true
      color=Colors.gray8
    />
    <Link className=Styles.seeAll route=Route.TxHomePage>
      <div className=Styles.cFlex>
        {switch (allSub) {
         | ApolloHooks.Subscription.Data((_, totalCount)) =>
           <Text
             value={totalCount |> Format.iPretty}
             size=Text.Xxl
             color=Colors.gray8
             height={Text.Px(24)}
             weight=Text.Bold
           />
         | _ => <LoadingCensorBar width=90 height=18 />
         }}
        <VSpacing size=Spacing.xs />
        <Text
          value="ALL TRANSACTIONS"
          size=Text.Sm
          color=Colors.bandBlue
          spacing={Text.Em(0.05)}
          weight=Text.Medium
        />
      </div>
      <img src=Images.rightArrow className=Styles.rightArrow />
    </Link>
  </div>;
};

let renderMobileTitle = allSub => {
  <div className=Styles.topicBar>
    <div>
      <Text
        value="Latest Transactions"
        size=Text.Xxl
        weight=Text.Bold
        block=true
        color=Colors.gray8
      />
      <div className=Styles.cFlex>
        {switch (allSub) {
         | ApolloHooks.Subscription.Data((_, totalCount)) =>
           <Text
             value={totalCount |> Format.iPretty}
             size=Text.Xxl
             color=Colors.gray8
             height={Text.Px(18)}
             weight=Text.Bold
           />
         | _ => <LoadingCensorBar width=90 height=18 />
         }}
      </div>
    </div>
    <Link className=Styles.seeAll route=Route.TxHomePage>
      <img src=Images.rightArrow className=Styles.rightArrow />
      <Text
        value="ALL TRANSACTIONS"
        size=Text.Sm
        color=Colors.bandBlue
        spacing={Text.Em(0.05)}
        height={Text.Px(14)}
        weight=Text.Medium
      />
    </Link>
  </div>;
};

let renderBody = (reserveIndex, txSub: ApolloHooks.Subscription.variant(TxSub.t)) => {
  <TBody
    key={
      switch (txSub) {
      | Data({txHash}) => txHash |> Hash.toHex
      | _ => reserveIndex |> string_of_int
      }
    }>
    <Row minHeight={`px(30)} alignItems=`flexStart>
      <Col> <HSpacing size={`px(12)} /> </Col>
      <Col size=1.2>
        <VSpacing size=Spacing.sm />
        {switch (txSub) {
         | Data({txHash}) => <TxLink txHash width=110 />
         | _ => <LoadingCensorBar width=100 height=10 />
         }}
      </Col>
      <Col size=0.68>
        <VSpacing size=Spacing.sm />
        {switch (txSub) {
         | Data({blockHeight}) =>
           <div className=Styles.blockContainer> <TypeID.Block id=blockHeight /> </div>
         | _ => <LoadingCensorBar width=50 height=10 />
         }}
      </Col>
      <Col size=1.>
        <VSpacing size=Spacing.xs />
        <div className=Styles.statusContainer>
          {switch (txSub) {
           | Data({success}) =>
             <img src={success ? Images.success : Images.fail} className=Styles.logo />
           | _ => <LoadingCensorBar width=20 height=20 radius=20 />
           }}
        </div>
      </Col>
      <Col size=3.8>
        {switch (txSub) {
         | Data({messages, txHash, success, errMsg}) =>
           <TxMessages txHash messages success errMsg width=360 />
         | _ => <> <VSpacing size=Spacing.sm /> <LoadingCensorBar width=360 height=10 /> </>
         }}
      </Col>
      <Col> <HSpacing size={`px(12)} /> </Col>
    </Row>
  </TBody>;
};

let renderBodyMobile = (reserveIndex, txSub: ApolloHooks.Subscription.variant(TxSub.t)) => {
  switch (txSub) {
  | Data({txHash, blockHeight, success, messages, errMsg}) =>
    <MobileCard
      values=InfoMobileCard.[
        ("TX HASH", TxHash(txHash, Media.isSmallMobile() ? 170 : 200)),
        ("BLOCK", Height(blockHeight)),
        ("ACTIONS", Messages(txHash, messages, success, errMsg)),
      ]
      key={txHash |> Hash.toHex}
      idx={txHash |> Hash.toHex}
      status=success
    />
  | _ =>
    <MobileCard
      values=InfoMobileCard.[
        ("TX HASH", Loading(Media.isSmallMobile() ? 170 : 200)),
        ("BLOCK", Loading(70)),
        (
          "ACTIONS",
          Loading(
            {
              Media.isSmallMobile() ? 160 : 230;
            },
          ),
        ),
      ]
      key={reserveIndex |> string_of_int}
      idx={reserveIndex |> string_of_int}
    />
  };
};

[@react.component]
let make = () => {
  let isMobile = Media.isMobile();
  let txCount = isMobile ? 5 : 10;
  let allSub = Sub.all2(TxSub.getList(~page=1, ~pageSize=txCount, ()), TxSub.count());
  <>
    {isMobile ? renderMobileTitle(allSub) : renderTitle(allSub)}
    <VSpacing size=Spacing.lg />
    {isMobile
       ? React.null
       : <THead>
           <Row>
             <Col> <HSpacing size={`px(12)} /> </Col>
             <Col size=1.2>
               <div className=Styles.fullWidth>
                 <Text
                   value="TX HASH"
                   size=Text.Sm
                   weight=Text.Semibold
                   color=Colors.gray6
                   spacing={Text.Em(0.05)}
                 />
               </div>
             </Col>
             <Col size=0.68>
               <div className={Css.merge([Styles.fullWidth, Styles.blockContainer])}>
                 <Text
                   value="BLOCK"
                   size=Text.Sm
                   weight=Text.Semibold
                   color=Colors.gray6
                   spacing={Text.Em(0.05)}
                 />
               </div>
             </Col>
             <Col size=1.>
               <div className=Styles.statusContainer>
                 <Text
                   value="STATUS"
                   size=Text.Sm
                   weight=Text.Semibold
                   color=Colors.gray6
                   spacing={Text.Em(0.05)}
                 />
               </div>
             </Col>
             <Col size=3.8>
               <div className=Styles.fullWidth>
                 <Text
                   value="ACTIONS"
                   size=Text.Sm
                   weight=Text.Semibold
                   color=Colors.gray6
                   spacing={Text.Em(0.05)}
                 />
               </div>
             </Col>
             <Col> <HSpacing size={`px(12)} /> </Col>
           </Row>
         </THead>}
    {switch (allSub) {
     | Data((txs, _)) =>
       txs
       ->Belt_Array.mapWithIndex((i, e) =>
           isMobile ? renderBodyMobile(i, Sub.resolve(e)) : renderBody(i, Sub.resolve(e))
         )
       ->React.array
     | _ =>
       Belt_Array.make(txCount, ApolloHooks.Subscription.NoData)
       ->Belt_Array.mapWithIndex((i, noData) =>
           isMobile ? renderBodyMobile(i, noData) : renderBody(i, noData)
         )
       ->React.array
     }}
  </>;
};
