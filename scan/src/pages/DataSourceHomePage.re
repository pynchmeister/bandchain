module Styles = {
  open Css;

  let vFlex = style([display(`flex), flexDirection(`row), alignItems(`center)]);

  let seperatedLine =
    style([
      width(`px(13)),
      height(`px(1)),
      marginLeft(`px(10)),
      marginRight(`px(10)),
      backgroundColor(Colors.gray7),
    ]);

  let textContainer = style([paddingLeft(Spacing.lg), display(`flex)]);

  let logo = style([width(`px(50)), marginRight(`px(10))]);

  let proposerBox = style([maxWidth(`px(270)), display(`flex), flexDirection(`column)]);

  let fullWidth = style([width(`percent(100.0)), display(`flex)]);

  // let feeContainer = style([display(`flex), justifyContent(`flexEnd), maxWidth(`px(150))]);
  let loadingContainer =
    style([
      display(`flex),
      justifyContent(`center),
      alignItems(`center),
      height(`px(200)),
      boxShadow(Shadow.box(~x=`zero, ~y=`px(2), ~blur=`px(2), Css.rgba(0, 0, 0, 0.05))),
      backgroundColor(white),
    ]);
};

let renderBody =
    (reserveIndex, dataSourcesSub: ApolloHooks.Subscription.variant(DataSourceSub.t)) => {
  Js.Console.log(dataSourcesSub);
  <TBody
    key={
      switch (dataSourcesSub) {
      | Data({id}) => id |> ID.DataSource.toString
      | _ => reserveIndex |> string_of_int
      }
    }>
    <Row minHeight={`px(30)}>
      <Col> <HSpacing size=Spacing.md /> </Col>
      <Col size=2.>
        {switch (dataSourcesSub) {
         | Data({id, name}) =>
           <> <TypeID.DataSource id /> <HSpacing size=Spacing.sm /> <Text value=name /> </>
         | _ => <LoadingCensorBar width=65 height=15 />
         }}
      </Col>
      <Col size=4.>
        {switch (dataSourcesSub) {
         | Data({description}) =>
           <Text value=description weight=Text.Medium block=true ellipsis=true />
         | _ => <LoadingCensorBar width=300 height=15 />
         }}
      </Col>
      <Col size=2.>
        // TODO it has no data for Request, now it has been mocked to 2000

          {switch (dataSourcesSub) {
           | Data(_) =>
             <div> <Text value="2000" weight=Text.Medium block=true ellipsis=true /> </div>
           | _ => <LoadingCensorBar width=150 height=15 />
           }}
        </Col>
      <Col size=2.>
        {switch (dataSourcesSub) {
         | Data({timestamp}) =>
           <Timestamp time=timestamp size=Text.Md weight=Text.Regular code=true />
         | _ => <LoadingCensorBar width=150 height=15 />
         }}
      </Col>
    </Row>
  </TBody>;
};

let renderBodyMobile =
    (reserveIndex, dataSourcesSub: ApolloHooks.Subscription.variant(DataSourceSub.t)) => {
  switch (dataSourcesSub) {
  | Data({id, timestamp, description, name}) =>
    <MobileCard
      values=InfoMobileCard.[
        ("Data Sourse", DataSources(id, name)),
        ("Description", Text(description)),
        ("Request", Count(2000)),
        ("Timestamp", Timestamp(timestamp)),
      ]
      key={id |> ID.DataSource.toString}
      idx={id |> ID.DataSource.toString}
    />
  | _ =>
    <MobileCard
      values=InfoMobileCard.[
        ("Data Sources", Loading(70)),
        ("Description", Loading(136)),
        ("Request", Loading(20)),
        ("Timestamp", Loading(166)),
      ]
      key={reserveIndex |> string_of_int}
      idx={reserveIndex |> string_of_int}
    />
  };
};

[@react.component]
let make = () => {
  let (page, setPage) = React.useState(_ => 1);
  let pageSize = 10;

  let dataSourcesCountSub = DataSourceSub.count();
  let dataSourcesSub = DataSourceSub.getList(~pageSize, ~page, ());

  // let%Sub dataSourcesCount = dataSourcesCountSub;
  // let%Sub dataSources = dataSourcesSub;

  let allSub = Sub.all2(DataSourceSub.getList(~pageSize, ~page, ()), DataSourceSub.count());
  let isMobile = Media.isMobile();
  <>
    // <div className=Styles.seperatedLine />
    //   <Text
    //     value={dataSourcesCount->string_of_int ++ " In total"}
    //     size=Text.Md
    //     weight=Text.Thin
    //     spacing={Text.Em(0.06)}
    //     color=Colors.gray7
    //     nowrap=true
    //   />
    // </div>
    <Heading value="All Data Sources" size=Heading.H2 />
    <>
      <THead>
        <Row>
          <Col> <HSpacing size=Spacing.xl /> </Col>
          <Col size=0.5>
            <div className=TElement.Styles.hashContainer>
              <Text
                block=true
                value="Data Sources"
                size=Text.Md
                weight=Text.Semibold
                color=Colors.gray5
              />
            </div>
          </Col>
          <Col size=0.5>
            <Text
              block=true
              value="Descriptions"
              size=Text.Md
              weight=Text.Semibold
              color=Colors.gray5
            />
          </Col>
          <Col size=1.>
            <Text
              block=true
              value="Timestamp"
              size=Text.Md
              weight=Text.Semibold
              color=Colors.gray5
            />
          </Col>
          <Col> <HSpacing size=Spacing.xl /> </Col>
        </Row>
      </THead>
      // {dataSources
      //  ->Belt_Array.map(({id, name, timestamp, owner}) => {
      //      <TBody key=name>
      //        <div className=Styles.fullWidth>
      //          <Row>
      //            <Col> <HSpacing size=Spacing.xl /> </Col>
      //            <Col size=0.5> <TElement elementType={TElement.DataSource(id, name)} /> </Col>
      //            <Col size=0.5> <TElement elementType={timestamp->TElement.Timestamp} /> </Col>
      //            <Col size=1.> <TElement elementType={owner->TElement.Address} /> </Col>
      //          </Row>
      //        </div>
      //      </TBody>
      //    })
      //  ->React.array}
    </>
    {switch (allSub) {
     | Data((dataSources, dataSourcesCount)) =>
       let pageCount = Page.getPageCount(dataSourcesCount, pageSize);
       <>
         {dataSources
          ->Belt_Array.mapWithIndex((i, e) =>
              isMobile ? renderBodyMobile(i, Sub.resolve(e)) : renderBody(i, Sub.resolve(e))
            )
          ->React.array}
         {isMobile
            ? React.null
            : <>
                <VSpacing size=Spacing.lg />
                <Pagination
                  currentPage=page
                  pageCount
                  onPageChange={newPage => setPage(_ => newPage)}
                />
                <VSpacing size=Spacing.lg />
              </>}
       </>;
     | _ =>
       Belt_Array.make(10, ApolloHooks.Subscription.NoData)
       ->Belt_Array.mapWithIndex((i, noData) =>
           isMobile ? renderBodyMobile(i, noData) : renderBody(i, noData)
         )
       ->React.array
     }}
  </>;
};
