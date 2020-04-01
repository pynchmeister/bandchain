module ValidatorReport = {
  type oracle_script_t = {
    id: ID.OracleScript.t,
    name: string,
  };

  type request_t = {
    id: ID.Request.t,
    oracleScript: oracle_script_t,
  };

  type report_details_t = {
    dataSourceID: ID.DataSource.t,
    externalID: int,
    data: JsBuffer.t,
  };

  type t = {
    txHash: Hash.t,
    request: request_t,
    reportDetails: array(report_details_t),
  };

  module MultiConfig = [%graphql
    {|
      subscription Reports ($limit: Int!, $offset: Int!, $validator: String!) {
        reports (where: {validator: {_eq: $validator}}, limit: $limit, offset: $offset) @bsRecord {
            request @bsRecord {
              id @bsDecoder (fn: "ID.Request.fromJson")
              oracleScript: oracle_script @bsRecord {
                id @bsDecoder (fn: "ID.OracleScript.fromJson")
                name
              }
            }
            txHash: tx_hash @bsDecoder (fn: "GraphQLParser.hash")
            reportDetails: report_details @bsRecord {
              dataSourceID: data_source_id @bsDecoder (fn: "ID.DataSource.fromJson")
              externalID: external_id @bsDecoder (fn: "GraphQLParser.int64")
              data @bsDecoder (fn: "GraphQLParser.buffer")
            }
          }
        }
      |}
  ];

  let getListByValidator = (~page=1, ~pageSize=5, ~validator) => {
    let offset = (page - 1) * pageSize;
    let (result, _) =
      ApolloHooks.useSubscription(
        MultiConfig.definition,
        ~variables=MultiConfig.makeVariables(~limit=pageSize, ~offset, ~validator, ()),
      );
    result |> Sub.map(_, x => x##reports);
  };
  // module MultiConfig = [%graphql
  //   {|
  //     subscription Reports($validator_address: String!, $limit: Int!, $offset: Int!) {
  //       reports(where: {validator:{_eq: $validator_address}}, limit: $limit, offset:$offset) @bsRecord{
  //         txHash: tx_hash @bsDecoder(fn: "Address.fromBech32")
  //       }
  //     }
  //   |}
  // ];
};
